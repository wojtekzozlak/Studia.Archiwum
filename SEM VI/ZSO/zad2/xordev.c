/* Sterownik do urządzenia XorDev */

/* Kernel includes */
#include <linux/module.h>
#include <linux/init.h> /* module_init, modul_exit */
#include <asm/uaccess.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/major.h>
#include <linux/kernel.h> /* KERN_WARNING */
#include <linux/pci.h>
#include <linux/cdev.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/random.h>

#define DEBUG true
#define DBG if(DEBUG)

#define DRVNAME "xordev"
#define MAX_DEVICES 64
#define BUFFER_SIZE 8192

#define BAR_INTR_EN 0x00
#define BAR_SRC1  0x04
#define BAR_SRC2  0x08
#define BAR_DST   0x0c
#define BAR_COUNT 0x10
#define BAR_INTR_COUNT 0x14

MODULE_AUTHOR("Wojciech Zoltak <wz292593@students.mimuw.edu.pl>");
MODULE_DESCRIPTION("Driver for the Xor device.");
MODULE_LICENSE("GPL");

/* global structures mutex */
static DEFINE_MUTEX(global_lock);

/* chrdev related variables */

static int major;
static struct class *xordev_class;

static struct xordev_str {
  dev_t devno;                 /* device number             */
  struct mutex lock;           /* mutex                     */
  spinlock_t spinlock;         /* spinlock                  */
  struct cdev cdev[3];         /* char device               */
  struct device *dev[3];       /* udev device               */

  uint32_t offset[3];          /* offsets for buffers       */
  char *cpu_addr[3];           /* CPU addresses of buffers  */
  dma_addr_t dma_addr[3];      /* DMA addresses of buffers  */
  uint32_t scheduled;          /* bytes scheduled in device */
  void __iomem *bar;           /* device BAR address        */
  char rdy_read, rdy_write[2]; /* workers notifications     */
  wait_queue_head_t queue;     /* workers queue             */
} xordev_data[MAX_DEVICES];

static char *dev_names[3] = {"xor%ds1", "xor%ds2", "xor%dd"};


/* ------ PART 1 - device operations ------ */

static void
try_schedule(struct xordev_str *xordev)
/* Function tries to schedule a new work on our device, by checking *
 * buffers offsets and the device state.                            */
{
  uint32_t i, count;

  spin_lock(&xordev->spinlock);

  count = ioread32(xordev->bar + BAR_COUNT); /* ile zostalo do przerobienia */
  if (xordev->scheduled % BUFFER_SIZE == 0 && count == 0) {
    /* reached end of buffer (or not started yet), device is not working, *
     * so we're resetting buffers                                         */
    iowrite32(xordev->dma_addr[0], xordev->bar + BAR_SRC1);
    iowrite32(xordev->dma_addr[1], xordev->bar + BAR_SRC2);
    iowrite32(xordev->dma_addr[2], xordev->bar + BAR_DST);
  }
  i = min(xordev->offset[0], xordev->offset[1]) - xordev->scheduled;

  if (xordev->scheduled <= BUFFER_SIZE && xordev->scheduled + i > BUFFER_SIZE) {
    /* cross-bound bytes, schedule only to the end off buffer */
    i = BUFFER_SIZE - xordev->scheduled;
  } else if (xordev->scheduled == BUFFER_SIZE && count > 0) {
    i = 0; /* can't schedule more :( */
  }

  if (i > 0) {
    iowrite32(i, xordev->bar + BAR_COUNT);
    iowrite32(1, xordev->bar + BAR_INTR_EN);
  }
  xordev->scheduled += i;

  spin_unlock(&xordev->spinlock);

  return ;
}

static int
xordev_read(struct file *filp, char __user *buff, size_t count, loff_t *offp)
/* Reader handler for our device.                                          *
 *                                                                         *
 * Serves as much data as it's possible in current state. When no data is  *
 * provided waits for data to occur, then serves as much as it's possible. */
{
  struct xordev_str *xordev = (struct xordev_str*) filp->private_data;
  uint32_t dst, cnt, bound, offset = 0, i, ret = 0, read, err;

  mutex_lock(&xordev->lock);

  do {
    cnt = ioread32(xordev->bar + BAR_COUNT);
    dst = ioread32(xordev->bar + BAR_DST) - xordev->dma_addr[2];
    read = xordev->offset[2]; /* one read, spinlock not needed */

    for(i = 0; i < 2; i++){
      /* first pass - from read to end/dst *
       * second     - from begining to dst */
      bound = (dst <= BUFFER_SIZE ? dst : BUFFER_SIZE);
      if (read > bound)
        bound += BUFFER_SIZE;
      offset = min(bound - read, count);
      err = copy_to_user(buff + ret, xordev->cpu_addr[2] + read, offset);
      if (err != 0) {
        offset = err; /* pretend that nothing happened, 'll check at the end */
        printk(KERN_ERR "copy_to_user: %d", err);
      }

      read += offset;
      ret += offset;
      if (read >= BUFFER_SIZE) {
        /* Reader has just crossed the buffer end. Reset the counters. */
        read %= BUFFER_SIZE;
        dst %= BUFFER_SIZE;

        spin_lock(&xordev->spinlock);
        xordev->offset[0] %= BUFFER_SIZE;
        xordev->offset[1] %= BUFFER_SIZE;
        xordev->scheduled %= BUFFER_SIZE;
        spin_unlock(&xordev->spinlock);
      }
      if (ret == count) /* everything written */
        break;
    }

    if (ret == 0) {
      mutex_unlock(&xordev->lock);
      /* wait for a sign, that we may have work to do, then try to read */
      wait_event_interruptible(xordev->queue, xordev->rdy_read);
      mutex_lock(&xordev->lock);
      xordev->rdy_read = false;
    } else break;
  } while(true);

  *offp = filp->f_pos + ret;
  try_schedule(xordev);

  spin_lock(&xordev->spinlock);
  xordev->offset[2] = read;
  spin_unlock(&xordev->spinlock);

  xordev->rdy_read = xordev->rdy_write[0] = xordev->rdy_write[1] = true;
  wake_up_all(&xordev->queue); /* get up! */
  mutex_unlock(&xordev->lock);

  return (ret > 0 ? ret : -EIO);
}

static ssize_t
xordev_write_n(int n, struct file *filp, const char __user *buff,
               size_t count, loff_t *offp)
/* Writer handler for our device.                                         *
 *                                                                        *
 * Writes as much data as it's possible. When buffer is full, waits until *
 * someone free it, than writes as much as it's possible.                 */
{
  struct xordev_str *xordev = (struct xordev_str*) filp->private_data;
  uint32_t src, read, offset = 0, offset2 = 0, err;
  char *dev_buff = xordev->cpu_addr[n];

  mutex_lock(&xordev->lock);

  do {
    spin_lock(&xordev->spinlock);
    src = xordev->offset[n]; /* our buffer offset */
    read = xordev->offset[2]; /* reader buffer offset */
    spin_unlock(&xordev->spinlock);

    /* jesli miejsce do konca bufora, to zapisujemy */
    if (src < BUFFER_SIZE) {
      offset = min(BUFFER_SIZE - src, count); /* min(free_end, data_length) */
      err = copy_from_user(dev_buff + src, buff, offset);
      if (err != 0){
        printk(KERN_ERR "copy_from_user: %d", err);
        offset = err;
        break;
      }
      src += offset;
    }
    /* jesli cos zostalo, to sprawdzamy, czy mozemy napisac na poczatku bufora. */
    if (offset != count && read > src % BUFFER_SIZE) {
      /* min(free_begin, data_length) */
      offset2 = min(read - src % BUFFER_SIZE, count - offset);
      err = copy_from_user(dev_buff + (src % BUFFER_SIZE), buff + offset, offset2);
      if (err != 0){
        printk(KERN_ERR "copy_from_user: %d", err);
        offset2 = err;
        break;
      }
      src += offset2;
    }
    /* no place, go to sleep */
    if (offset + offset2 == 0) {
      mutex_unlock(&xordev->lock);
      /* wait for a sign, then try to write */
      err = wait_event_interruptible(xordev->queue, xordev->rdy_write[n]);
      mutex_lock(&xordev->lock);
      xordev->rdy_write[n] = false;
    } else break;
  } while(true);

  offset += offset2;
  *offp = filp->f_pos + offset;

  if (offset > 0) {
    spin_lock(&xordev->spinlock);
    xordev->offset[n] = src;
    spin_unlock(&xordev->spinlock);

    /* try to schedule a job */
    try_schedule(xordev);

    xordev->rdy_read = xordev->rdy_write[0] = xordev->rdy_write[1] = true;
    wake_up_all(&xordev->queue); /* get up! */
    mutex_unlock(&xordev->lock);
  }

  return (offset > 0 ? offset : EIO);
}

static ssize_t
xordev_write1(struct file *filp, const char __user *buff,
              size_t count, loff_t *offp)
/* Handler for xorXs1 writers. */
{
  return xordev_write_n(0, filp, buff, count, offp);
}

static ssize_t
xordev_write2(struct file *filp, const char __user *buff,
              size_t count, loff_t *offp)
/* Handler for xorXs2 writers. */
{
  return xordev_write_n(1, filp, buff, count, offp);
}

static int xordev_open(struct inode *inode, struct file *filp);
static int xordev_release(struct inode *inode, struct file *filep);

static struct file_operations xordev_fops = {
  .owner = THIS_MODULE,
  .open = xordev_open,
  .read = xordev_read,
  .release = xordev_release,
};

static struct file_operations xordev_wr1_fops = {
  .owner = THIS_MODULE,
  .open = xordev_open,
  .write = xordev_write1,
  .release = xordev_release
};

static struct file_operations xordev_wr2_fops = {
  .owner = THIS_MODULE,
  .open = xordev_open,
  .write = xordev_write2,
  .release = xordev_release
};

static int
xordev_release(struct inode *inode, struct file *filep){
  return 0;
}

static int
xordev_open(struct inode *inode, struct file *filp)
/* Open operation handler.                   *
 *                                           *
 * Checks file type and assigns proper FOPs. */
{
  switch (MINOR(inode->i_rdev) % 3) {
    case 0: /* xorXs1 */
      if (filp->f_mode & FMODE_READ || !(filp->f_mode & FMODE_WRITE)) {
        return -EACCES;
      }
      filp->f_op = &xordev_wr1_fops;
      break;
    case 1: /* xorXs2 */
      if (filp->f_mode & FMODE_READ || !(filp->f_mode & FMODE_WRITE)) {
        return -EACCES;
      }
      filp->f_op = &xordev_wr2_fops;
      break;
    case 2: /* xorXd */
      if (!(filp->f_mode & FMODE_READ) || filp->f_mode & FMODE_WRITE) {
        return -EACCES;
      }
      break;
    default:
      break;
  }
  filp->private_data = &xordev_data[MINOR(inode->i_rdev) / 3];

  return 0;
}


/* ------ PART 2 - PCI DEVICE ------ */

/* PCI device structures */

static int xordev_probe(struct pci_dev *dev, const struct pci_device_id *id);
void xordev_remove(struct pci_dev *dev);

static DEFINE_PCI_DEVICE_TABLE(pci_devices) = {
  { PCI_DEVICE(0x1af4, 0x10fd) },
  { 0, }, /* list termination */
};

static struct pci_driver xordev_pci_driver = {
  .name = "xordev",
  .id_table = pci_devices,
  .probe = xordev_probe,
  .remove = xordev_remove,
  .shutdown = xordev_remove
};


/* PCI device handlers */

static irqreturn_t
xordev_irq(int irq, void *dev)
/* Interruption handler. */
{
  struct xordev_str *xordev = (struct xordev_str *) dev;

  spin_lock(&xordev->spinlock);

  /* checking if it's our interruption */
  if (ioread32(xordev->bar + BAR_INTR_EN) == 1 &&
      ioread32(xordev->bar + BAR_COUNT) == 0)
  {
    iowrite32(0, xordev->bar + BAR_INTR_EN); /* disable intr */
    /* inform workers that there may be work to do */
    xordev->rdy_read = xordev->rdy_write[0] = xordev->rdy_write[1] = true;
    wake_up_all(&xordev->queue);

    spin_unlock(&xordev->spinlock);
    return IRQ_HANDLED;
  }

  spin_unlock(&xordev->spinlock);
  return IRQ_NONE;
}


static int
xordev_probe(struct pci_dev *dev, const struct pci_device_id *id)
/* PCI device initializer */
{
  uint32_t i, j;
  int err = 0;
  struct xordev_str *xordev;
  dev_t devt;

  mutex_lock(&global_lock);

  /* find empty slot and initialize */
  for (i = 0; xordev_data[i].devno != 0 && i < MAX_DEVICES; i++);
  if (i == MAX_DEVICES){
    printk (KERN_NOTICE "Too many xordev devices!");
    return -1;
  }
  xordev = &xordev_data[i];
  memset(xordev, 0, sizeof(struct xordev_str)); /* clear everything */

  xordev->devno = MKDEV(major, i * 3);
  mutex_init(&xordev->lock);
  xordev->spinlock = SPIN_LOCK_UNLOCKED;

  for (j = 0; j < 3; j++) {
    devt = MKDEV(major, i * 3 + j);

    /* creating char device */
    cdev_init(&xordev->cdev[j], &xordev_fops);
    xordev->cdev[j].ops = &xordev_fops;
    err = cdev_add(&xordev->cdev[j], devt, 1);
    if (err) {
      printk(KERN_NOTICE "Error %d adding XorDev %d", err, i);
      goto fail;
    }

    /* udev device create */
    xordev->dev[j] = device_create(xordev_class, 0, devt,
                                   xordev, dev_names[j], i);
    if (IS_ERR(xordev->dev[j])) {
      err = PTR_ERR(xordev->dev[j]);
      printk(KERN_NOTICE "Error %d creating udev device", err);
      goto udev_create_fail;
    }
  }

  pci_set_drvdata(dev, xordev);
  /* enable PCI device */
  err = pci_enable_device(dev);
  if (err) {
    printk(KERN_NOTICE "Error %d enabling XorDev PCI device %d", err, i);
    goto pci_enable_fail;
  }
  err = pci_request_regions(dev, DRVNAME);
  if (err) {
    printk(KERN_NOTICE "Error %d requesting XorDev %d regions", err, i);
    goto pci_regions_fail;
  }
  xordev->bar = pci_iomap(dev, 0, 4096);
  if (NULL == xordev->bar) {
    printk(KERN_NOTICE "Can't map XorDev %d iomem", i);
    goto pci_iomap_fail;
  }
  /* enable DMA */
  pci_set_master(dev);
  err = pci_set_dma_mask(dev, DMA_BIT_MASK(32));
  if (err) {
    printk(KERN_NOTICE "Can't set DMA mask for XorDev %d", i);
    goto pci_iomap_fail;
  }
  err = pci_set_consistent_dma_mask(dev, DMA_BIT_MASK(32));
  if (err) {
    printk(KERN_NOTICE "Can't set consistent DMA mask for XorDev %d", i);
    goto pci_iomap_fail;
  }
  /* alloc buffers */
  xordev->cpu_addr[0] = dma_alloc_coherent(&dev->dev, BUFFER_SIZE * 3,
                                           &xordev->dma_addr[0], 0);
  if (xordev->cpu_addr) {
    xordev->cpu_addr[1] = xordev->cpu_addr[0] + BUFFER_SIZE;
    xordev->dma_addr[1] = xordev->dma_addr[0] + BUFFER_SIZE;
    xordev->cpu_addr[2] = xordev->cpu_addr[1] + BUFFER_SIZE;
    xordev->dma_addr[2] = xordev->dma_addr[1] + BUFFER_SIZE;
    memset(xordev->offset, 0, 12);
    iowrite32(xordev->dma_addr[0], xordev->bar + BAR_SRC1);
    iowrite32(xordev->dma_addr[1], xordev->bar + BAR_SRC2);
    iowrite32(xordev->dma_addr[2], xordev->bar + BAR_DST);
  } else {
    printk(KERN_NOTICE "Can't allocate DMA memory for XorDev %d", i);
    goto pci_iomap_fail;
  }
  /* waiting queue */
  init_waitqueue_head(&xordev->queue);
  /* interruptions */
  err = request_irq(dev->irq, xordev_irq, IRQF_SHARED, DRVNAME, xordev);
  if (err) {
    printk(KERN_NOTICE "Can't register irq handler for XorDev %d", i);
    goto request_irq_fail;
  }

  mutex_unlock(&global_lock);
  return 0;

request_irq_fail:
  pci_iounmap(dev, xordev->bar);
pci_iomap_fail:
  pci_release_regions(dev);
pci_regions_fail:
  pci_disable_device(dev);
pci_enable_fail:
  for (i = 0; i < j; i++)
    device_destroy(xordev_class,
                   MKDEV(MAJOR(xordev->devno), MINOR(xordev->devno) + i));
udev_create_fail:
  for (i = 0; i < j; i++)
    cdev_del(&xordev->cdev[j]);
fail:
  mutex_destroy(&xordev->mutex);
  xordev->devno = 0;
  mutex_unlock(&global_lock);
  return err;
}

void
xordev_remove(struct pci_dev *dev)
/* PCI device destructor */
{
  struct xordev_str *xordev;
  int j;
  dev_t devt;

  mutex_lock(&global_lock);

  xordev = pci_get_drvdata(dev);
  iowrite32(0, xordev->bar + BAR_INTR_EN); /* disable interruptions */
  free_irq(dev->irq, xordev);
  pci_iounmap(dev, xordev->bar);
  pci_release_regions(dev);
  pci_disable_device(dev);
  for (j = 0; j < 3; j++) {
    devt = MKDEV(MAJOR(xordev->devno), MINOR(xordev->devno) + j);
    device_destroy(xordev_class, devt);
    cdev_del(&xordev->cdev[j]);
  }
  mutex_destroy(&xordev->mutex);

  mutex_unlock(&global_lock);
  return ;
}

/* ------ PART 3 - kernel module ------ */

/* kernel module handlers */

static void
xordev_cleanup_module(void)
{
  pci_unregister_driver(&xordev_pci_driver);
  unregister_chrdev(major, DRVNAME);
  class_destroy(xordev_class);
  return ;
}

static int
xordev_init_module(void)
{
  int result, i;

  /* clear structures */
  for(i = 0; i < MAX_DEVICES; i++) xordev_data[i].devno = 0;

  /* Registering character device */
  major = register_chrdev(0, DRVNAME, &xordev_fops);
  if (major < 0) {
    printk(KERN_NOTICE "Can't register a character device");
    result = major;
    goto fail;
  }
  xordev_class = class_create(THIS_MODULE, DRVNAME);
  if (IS_ERR(xordev_class)) {
    printk(KERN_NOTICE "Can't create device class");
    result = PTR_ERR(xordev_class);
    goto fail;
  }

  /* PCI probing */
  DBG printk(KERN_DEBUG "Probing PCI devices");
  result = pci_register_driver(&xordev_pci_driver);
  if (result < 0) {
    printk(KERN_NOTICE "Can't register pci driver");
    goto fail;
  }

  return 0; /* success */

fail:
  xordev_cleanup_module();
  return result;
}

module_init(xordev_init_module);
module_exit(xordev_cleanup_module);
