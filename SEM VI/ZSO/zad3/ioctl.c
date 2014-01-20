/*
 * linux/fs/ext2/ioctl.c
 *
 * Copyright (C) 1993, 1994, 1995
 * Remy Card (card@masi.ibp.fr)
 * Laboratoire MASI - Institut Blaise Pascal
 * Universite Pierre et Marie Curie (Paris VI)
 */

#include "ext2.h"
#include <linux/capability.h>
#include <linux/time.h>
#include <linux/sched.h>
#include <linux/compat.h>
#include <linux/mount.h>
#include <linux/buffer_head.h>
#include <asm/current.h>
#include <asm/uaccess.h>

long ext2_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	struct inode *inode = filp->f_dentry->d_inode;
	struct ext2_inode_info *ei = EXT2_I(inode);
	unsigned int flags;
	unsigned short rsv_window_size;
	int ret;

	ext2_debug ("cmd = %u, arg = %lu\n", cmd, arg);

	switch (cmd) {
  case EXT2_FAKE_B_ALLOC:
    /* Fake allocation for ext2 filesystem.
     * */
    {
      struct ext2_fake_b_alloc_arg config;
      struct buffer_head bh_result;
      sector_t iblock, off;
      int ret = 0;

      ret = copy_from_user(&config, (struct ext2_fake_b_alloc_arg __user *)arg,
                           sizeof(struct ext2_fake_b_alloc_arg));
      if (ret != 0) {
        printk (KERN_DEBUG "can't copy from user");
        return -EIO;
      } else ret = 0;

      /* Allocate blocks. */
      off = config.efba_off;
      iblock = config.efba_off >> inode->i_blkbits;
      while ((iblock << inode->i_blkbits) <
                  (config.efba_off + config.efba_size)) {
        memset(&bh_result, 0, sizeof(struct ext2_fake_b_alloc_arg));
        ret = ext2_get_block(inode, iblock, &bh_result, 1);
        if (ret < 0) {
          printk (KERN_DEBUG "get_block_error %d, escaping", ret);
          break;
        }
        iblock++;
      }

      /* Set metadata */
      write_lock(&EXT2_I(inode)->i_meta_lock);
      if (ret == 0) {
        printk (KERN_DEBUG "ok, set size");
        inode->i_size = max_t(loff_t, inode->i_size,
                            config.efba_off + config.efba_size);
      } else if(iblock != config.efba_off >> inode->i_blkbits) {
        /* Partially allocated, size must be fixed.           *
         * But `i_blocks` should containt actual information. */
        inode->i_size = inode->i_blocks << inode->i_blkbits;
      }
      inode->i_mtime = inode->i_atime = CURRENT_TIME_SEC;
      inode->i_version++;
      write_unlock(&EXT2_I(inode)->i_meta_lock);

      printk(KERN_DEBUG, "returning %d", ret);
      return ret;
    }

	case EXT2_IOC_GETFLAGS:
		ext2_get_inode_flags(ei);
		flags = ei->i_flags & EXT2_FL_USER_VISIBLE;
		return put_user(flags, (int __user *) arg);
	case EXT2_IOC_SETFLAGS: {
		unsigned int oldflags;

		ret = mnt_want_write(filp->f_path.mnt);
		if (ret)
			return ret;

		if (!is_owner_or_cap(inode)) {
			ret = -EACCES;
			goto setflags_out;
		}

		if (get_user(flags, (int __user *) arg)) {
			ret = -EFAULT;
			goto setflags_out;
		}

		flags = ext2_mask_flags(inode->i_mode, flags);

		mutex_lock(&inode->i_mutex);
		/* Is it quota file? Do not allow user to mess with it */
		if (IS_NOQUOTA(inode)) {
			mutex_unlock(&inode->i_mutex);
			ret = -EPERM;
			goto setflags_out;
		}
		oldflags = ei->i_flags;

		/*
		 * The IMMUTABLE and APPEND_ONLY flags can only be changed by
		 * the relevant capability.
		 *
		 * This test looks nicer. Thanks to Pauline Middelink
		 */
		if ((flags ^ oldflags) & (EXT2_APPEND_FL | EXT2_IMMUTABLE_FL)) {
			if (!capable(CAP_LINUX_IMMUTABLE)) {
				mutex_unlock(&inode->i_mutex);
				ret = -EPERM;
				goto setflags_out;
			}
		}

		flags = flags & EXT2_FL_USER_MODIFIABLE;
		flags |= oldflags & ~EXT2_FL_USER_MODIFIABLE;
		ei->i_flags = flags;
		mutex_unlock(&inode->i_mutex);

		ext2_set_inode_flags(inode);
		inode->i_ctime = CURRENT_TIME_SEC;
		mark_inode_dirty(inode);
setflags_out:
		mnt_drop_write(filp->f_path.mnt);
		return ret;
	}
	case EXT2_IOC_GETVERSION:
		return put_user(inode->i_generation, (int __user *) arg);
	case EXT2_IOC_SETVERSION:
		if (!is_owner_or_cap(inode))
			return -EPERM;
		ret = mnt_want_write(filp->f_path.mnt);
		if (ret)
			return ret;
		if (get_user(inode->i_generation, (int __user *) arg)) {
			ret = -EFAULT;
		} else {
			inode->i_ctime = CURRENT_TIME_SEC;
			mark_inode_dirty(inode);
		}
		mnt_drop_write(filp->f_path.mnt);
		return ret;
	case EXT2_IOC_GETRSVSZ:
		if (test_opt(inode->i_sb, RESERVATION)
			&& S_ISREG(inode->i_mode)
			&& ei->i_block_alloc_info) {
			rsv_window_size = ei->i_block_alloc_info->rsv_window_node.rsv_goal_size;
			return put_user(rsv_window_size, (int __user *)arg);
		}
		return -ENOTTY;
	case EXT2_IOC_SETRSVSZ: {

		if (!test_opt(inode->i_sb, RESERVATION) ||!S_ISREG(inode->i_mode))
			return -ENOTTY;

		if (!is_owner_or_cap(inode))
			return -EACCES;

		if (get_user(rsv_window_size, (int __user *)arg))
			return -EFAULT;

		ret = mnt_want_write(filp->f_path.mnt);
		if (ret)
			return ret;

		if (rsv_window_size > EXT2_MAX_RESERVE_BLOCKS)
			rsv_window_size = EXT2_MAX_RESERVE_BLOCKS;

		/*
		 * need to allocate reservation structure for this inode
		 * before set the window size
		 */
		/*
		 * XXX What lock should protect the rsv_goal_size?
		 * Accessed in ext2_get_block only.  ext3 uses i_truncate.
		 */
		mutex_lock(&ei->truncate_mutex);
		if (!ei->i_block_alloc_info)
			ext2_init_block_alloc_info(inode);

		if (ei->i_block_alloc_info){
			struct ext2_reserve_window_node *rsv = &ei->i_block_alloc_info->rsv_window_node;
			rsv->rsv_goal_size = rsv_window_size;
		}
		mutex_unlock(&ei->truncate_mutex);
		mnt_drop_write(filp->f_path.mnt);
		return 0;
	}
	default:
		return -ENOTTY;
	}
}

#ifdef CONFIG_COMPAT
long ext2_compat_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	/* These are just misnamed, they actually get/put from/to user an int */
	switch (cmd) {
	case EXT2_IOC32_GETFLAGS:
		cmd = EXT2_IOC_GETFLAGS;
		break;
	case EXT2_IOC32_SETFLAGS:
		cmd = EXT2_IOC_SETFLAGS;
		break;
	case EXT2_IOC32_GETVERSION:
		cmd = EXT2_IOC_GETVERSION;
		break;
	case EXT2_IOC32_SETVERSION:
		cmd = EXT2_IOC_SETVERSION;
		break;
	default:
		return -ENOIOCTLCMD;
	}
	return ext2_ioctl(file, cmd, (unsigned long) compat_ptr(arg));
}
#endif
