/* Wojciech Żółtak (wz292583) - Zero Blocks device
 *
 * Device is mainly based on a `brd.c` ram drive implementation. */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>

#include <linux/kernel.h> /* printk() */
#include <linux/fs.h>     /* everything... */
#include <linux/errno.h>  /* error codes */
#include <linux/types.h>  /* size_t */
#include <linux/vmalloc.h>
#include <linux/genhd.h>
#include <linux/blkdev.h>
#include <linux/hdreg.h>
#include <linux/gfp.h>
#include <linux/slab.h>
#include <linux/radix-tree.h>

#define SECTOR_SHIFT 9
#define ZERO_BDEV_CONFIGURE 0x66660667

#define ZB_INACTIVE 0
#define ZB_ACTIVE   1
#define ZB_REMOVED  2

#ifndef CONFIG_BLK_DEV_ZERO_COUNT
  #define CONFIG_BLK_DEV_ZERO_COUNT 4
#endif

MODULE_LICENSE("GPL");

static int num_devs = 0;
module_param(num_devs, int, 0);

static DEFINE_MUTEX(devices_mutex);

struct zero_bdev_config_arg
{
  uint32_t zbc_block_size;
  uint32_t zbc_pad_; //unused
  uint64_t zbc_num_blocks;
};

/* main zb device structure */
static struct zb_device {
  char zb_status;
  char zb_name[8];
  long zb_block_size;
  int  zb_block_shift; /* log_2(zb_block_size) */
  long zb_num_blocks;
  int  zb_minor;
  int  zb_major;

  struct radix_tree_root zb_data;
  struct rw_semaphore zb_sem;

  struct gendisk *zb_gd;
  struct request_queue *zb_queue;
} *devices;

/* single data block */
struct data_block {
  char *data;
  unsigned long ix;
};

static int
copy_from_zerod(void *dst, struct zb_device *dev,
                sector_t sector, uint32_t n)
/* Copy `n` bytes from zbd starting from sector `sector`. */
{
  uint32_t offset;
  uint64_t ix, buff;
  size_t copy;
  struct data_block* block;

  while (n > 0) {
    buff = sector << SECTOR_SHIFT;
    ix = buff >> dev->zb_block_shift;

    /* every request should be properly aligned */
    offset = buff - (ix * dev->zb_block_size);
    if (offset != 0) {
      printk (KERN_WARNING "%s: bad aligned request\n", dev->zb_name);
      return -EINVAL;
    }

    copy = min_t(size_t, dev->zb_block_size, n);
    rcu_read_lock();
    block = radix_tree_lookup(&dev->zb_data, ix);
    rcu_read_unlock();

    if (block) {
      memcpy(dst, block->data + offset, copy);
    } else {
      memset(dst, 0, copy);
    }

    n -= copy;
    dst += copy;
    sector += copy >> SECTOR_SHIFT;
  }

  return 0;
}

static int
check_zero(char *dst, uint32_t n)
/* Returns 1 if `n` bytes starting from `dst` are all zeros. */
{
  if (n == 0)
    return 1;
  if (dst[0] == 0 && memcmp(dst, dst + 1, n - 1) == 0)
    return 1;
  return 0;
}

static struct data_block*
alloc_block(struct zb_device *dev, uint64_t ix)
/* Returns a pointer to a new, empty data_block. */
{
  struct data_block *block = kzalloc(sizeof(struct data_block) + dev->zb_block_size,
                                     GFP_NOIO);
  block->data = ((void *) block) + sizeof(struct data_block);
  block->ix = (unsigned long) ix;
  if (!block)
    return NULL;

  if (radix_tree_preload(GFP_NOIO)) {
    kfree(block);
    return NULL;
  }

  if (radix_tree_insert(&dev->zb_data, ix, block)) {
    kfree(block);
    block = radix_tree_lookup(&dev->zb_data, ix);
    BUG_ON(!block);
  }
  radix_tree_preload_end();

  return block;
}

#define FREE_BATCH 16
void
cleanup_tree(struct zb_device *dev)
/* Removes all data from the radix tree. */
{
  unsigned long pos = 0;
  struct data_block *blocks[FREE_BATCH];
  int nr_pages;

  do {
    int i;
    nr_pages = radix_tree_gang_lookup(&dev->zb_data, (void **)blocks,
                                      pos, FREE_BATCH);

    for (i = 0; i < nr_pages; i++) {
      struct data_block *ret;
      BUG_ON(blocks[i]->ix < pos);
      pos = blocks[i]->ix;
      ret = radix_tree_delete(&dev->zb_data, pos);
      BUG_ON(!ret || ret != blocks[i]);
      kfree(blocks[i]);

    }
    pos++;
  } while(nr_pages > 0);
}

static int
copy_to_zerod(struct zb_device *dev, void *src,
              sector_t sector, uint32_t n)
/* Copy `n` bytes from userspace into zbd, starting from `sector`. */
{
  uint32_t offset;
  uint64_t ix, buff;
  size_t copy;
  struct data_block* block;

  while (n > 0) {
    buff = sector << SECTOR_SHIFT;
    ix = buff >> dev->zb_block_shift;

    offset = buff - (ix * dev->zb_block_size);
    if (offset != 0) {
      printk (KERN_WARNING "%s: bad aligned request\n", dev->zb_name);
      return -EINVAL;
    }

    copy = min_t(size_t, dev->zb_block_size, n);

    block = radix_tree_lookup(&dev->zb_data, ix);
    if (check_zero(src, copy)) {
      if (block) {
        radix_tree_delete(&dev->zb_data, ix);
        kfree(block);
      }
    } else {
      if (!block)
        block = alloc_block(dev, ix);
      memcpy(block->data + offset, src, copy);
    }

    n -= copy;
    src += copy;
    sector += copy >> SECTOR_SHIFT;
  }

  return 0;
}


static int
zerod_do_bvec(struct zb_device *dev, struct page *page,
              uint32_t len, uint32_t off, int rw,
              sector_t sector)
/* Do a bvec request. */
{
  void *mem;
  int err = 0;

  mem = kmap(page);
  if (rw == READ) {
    down_read(&dev->zb_sem);
    if (dev->zb_status == ZB_ACTIVE) {
      err = copy_from_zerod(mem + off, dev, sector, len);
    } else err = -ENXIO;
    up_read(&dev->zb_sem);
  } else {
    down_write(&dev->zb_sem);
    if (dev->zb_status == ZB_ACTIVE) {
      err = copy_to_zerod(dev, mem + off, sector, len);
    } else err = -ENXIO;
    up_write(&dev->zb_sem);
  }

  kunmap(page);

  return err;
}

static int
zerod_make_request(struct request_queue *q, struct bio *bio)
/* Callback function for request making.
 *
 * Tries to satisfy the request, without using a queue. */
{
  struct block_device *bdev = bio->bi_bdev;
  struct zb_device *dev = bdev->bd_disk->private_data;
  int rw;
  struct bio_vec *bvec;
  sector_t sector;
  int i;
  int err = -EIO;

  down_read(&dev->zb_sem);
  if (dev->zb_status == ZB_INACTIVE) {
    err = -EPERM;
    goto out;
  } else if (dev->zb_status == ZB_REMOVED) {
    err = -ENXIO;
    goto out;
  }

  sector = bio->bi_sector;
  if (sector + (bio->bi_size >> SECTOR_SHIFT) > get_capacity(bdev->bd_disk)) {
    printk (KERN_WARNING "%s: request beyond device capacity\n", dev->zb_name);
    up_read(&dev->zb_sem);
    goto out;
  }
  up_read(&dev->zb_sem);

  rw = bio_rw(bio);
  if (rw == READA)
    rw = READ;


  if ( (bio->bi_vcnt == 0) && (bio->bi_rw & BIO_RW_BARRIER) ) {
    /* Barrier request - pretend that everything is ok. */
    err = 0;
    goto out;
  }

  bio_for_each_segment(bvec, bio, i) {
    uint32_t len = bvec->bv_len;
    err = zerod_do_bvec(dev, bvec->bv_page, len,
                             bvec->bv_offset, rw, sector);
    if (err) {
      break;
    }
    sector += len >> SECTOR_SHIFT;
  }

out:
  bio_endio(bio, err);

  return 0;
}


int
find_shift(long block_size)
/* Returns log_2(`block_size`) or -1 when `block_size` isn't in form of 2^k. */
{
  int ret = 0;
  if (block_size < 0)
    return -1;

  while (block_size > 1) {
    if (block_size % 2 == 1)
      return -1;
    ret += 1;
    block_size >>= 1;
  }
  return ret;
}

int
zerod_ioctl(struct block_device *bdev,
     fmode_t mode,
     unsigned int ioctl_num,  /* number and param for ioctl */
     unsigned long ioctl_param)
/* Device ioctl handler. */
{
  struct zero_bdev_config_arg config;
  struct zb_device *dev = bdev->bd_disk->private_data;
  int shift;

  switch (ioctl_num) {
    case ZERO_BDEV_CONFIGURE:
      if (bdev->bd_openers > 1 || !down_write_trylock(&dev->zb_sem)) {
        printk (KERN_INFO "%s: can't change configuration - device is busy\n",
                dev->zb_name);
        return -EBUSY;
      }
      if (dev->zb_status == ZB_REMOVED) {
        return -ENXIO;
      }

      if (0 != copy_from_user(&config, (struct zero_bdev_config_arg __user *) ioctl_param,
                              sizeof(struct zero_bdev_config_arg))) {
        return -EIO;
      }
      shift = find_shift(config.zbc_block_size);
      if (shift == -1) {
        printk(KERN_WARNING "%s: %u is not a valid block size\n",
               dev->zb_name, config.zbc_block_size);
        return -EINVAL;
      } else dev->zb_block_shift = shift;

      dev->zb_status = ZB_ACTIVE;
      dev->zb_block_size = config.zbc_block_size;
      dev->zb_num_blocks = config.zbc_num_blocks;
      blk_queue_logical_block_size(dev->zb_queue, dev->zb_block_size);
      blk_queue_physical_block_size(dev->zb_queue, dev->zb_block_size);

      set_blocksize(bdev, dev->zb_block_size);
      set_capacity(dev->zb_gd,
                   (dev->zb_num_blocks * dev->zb_block_size) >> SECTOR_SHIFT);
      cleanup_tree(dev);

      printk(KERN_INFO "%s: new configuration %ld (block size) %ld (num blocks)",
             dev->zb_name, dev->zb_block_size, dev->zb_num_blocks);
      up_write(&dev->zb_sem);

      break;
  }
  return 0;
}

static struct block_device_operations zbd_ops = {
    .owner = THIS_MODULE,
    .ioctl = zerod_ioctl,
};


static int
init_one(struct zb_device *dev, int i)
/* Initializes a single zbd device. */
{
  sprintf(dev->zb_name, "zbd%d", i);
  dev->zb_major = register_blkdev(0, dev->zb_name);
  if (dev->zb_major <= 0) {
    printk (KERN_WARNING "%s: unable to get major number", dev->zb_name);
    goto fail;
  }

  dev->zb_status = ZB_INACTIVE;
  dev->zb_block_size = -1;
  dev->zb_block_shift = -1;
  dev->zb_num_blocks = -1;
  dev->zb_minor = i;

  INIT_RADIX_TREE(&dev->zb_data, GFP_ATOMIC);
  init_rwsem(&dev->zb_sem);

  /* requests queue */
  dev->zb_queue = blk_alloc_queue(GFP_KERNEL);
  if (!dev->zb_queue)
    goto fail;
  blk_queue_make_request(dev->zb_queue, zerod_make_request);
  blk_queue_ordered(dev->zb_queue, QUEUE_ORDERED_TAG, NULL);
  blk_queue_max_hw_sectors(dev->zb_queue, 1024);
  blk_queue_bounce_limit(dev->zb_queue, BLK_BOUNCE_ANY);
  dev->zb_queue->queuedata = dev;

  /* disk structure */
  dev->zb_gd = alloc_disk(1);
  if (!dev->zb_gd)
    goto fail_cleanup_queue;
  dev->zb_gd->major = dev->zb_major;
  dev->zb_gd->first_minor = 0;
  dev->zb_gd->fops = &zbd_ops;
  dev->zb_gd->private_data = dev;
  dev->zb_gd->flags |= GENHD_FL_SUPPRESS_PARTITION_INFO;
  sprintf(dev->zb_gd->disk_name, "%s", dev->zb_name);
  dev->zb_gd->queue = dev->zb_queue;

  add_disk(dev->zb_gd);

  printk(KERN_INFO "%s: initizalized\n", dev->zb_name);
  return 0;

fail_cleanup_queue:
  blk_cleanup_queue(dev->zb_queue);
fail:
  return -ENOMEM;
}

static int
drop_one(struct zb_device *dev)
/* Removes a zbd device. */
{

  down_write(&dev->zb_sem);

  dev->zb_status = ZB_REMOVED;
  del_gendisk(dev->zb_gd);
  put_disk(dev->zb_gd);
  if (dev->zb_queue != NULL)
    blk_cleanup_queue(dev->zb_queue);
  cleanup_tree(dev);
  unregister_blkdev(dev->zb_major, dev->zb_name);

  up_write(&dev->zb_sem);

  return 0;
}

static void
exit_from(int i);

static int
__init zerod_init(void)
/* Module initialization callback. */
{
  int i;

  if (!num_devs) {
    num_devs = CONFIG_BLK_DEV_ZERO_COUNT;
  }

  devices = kzalloc(sizeof(struct zb_device) * num_devs, GFP_KERNEL);
  if (devices == NULL)
    goto out;

  for (i = 0; i < num_devs; i++) {
    if(init_one(&devices[i], i) < 0)
      goto cleanup;
  }

  printk(KERN_INFO "zbd: module loaded\n");
  return 0;

cleanup:
  exit_from(i - 1);
out:
  return -ENOMEM;
}

static void
__exit zerod_exit(void)
/* Module removal callback. */
{
  printk(KERN_INFO "zbd: destroying %d device(s)\n", num_devs);
  exit_from(num_devs - 1);
}

static void
exit_from(int i)
/* Removes devices starting from "zbd`i`" down to "zbd0". */
{
  for (; i >= 0; i--) {
    printk(KERN_INFO "%s: dropping\n", devices[i].zb_name);
    drop_one(&devices[i]);
  }
  kfree(devices);
  printk(KERN_INFO "zbd: module unloaded\n");
}

module_init(zerod_init);
module_exit(zerod_exit);
