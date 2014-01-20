#ifndef _ZERO_BDEV_H_
#define _ZERO_BDEV_H_

#include <linux/types.h>

#define ZERO_BDEV_CONFIGURE 0x66660667
#define EXT2_FAKE_B_ALLOC   0x66660666

struct zero_bdev_config_arg
{
  uint32_t zbc_block_size;
  uint32_t zbc_pad_; //unused
  uint64_t zbc_num_blocks;
};

struct ext2_fake_b_alloc_arg
{
  uint32_t efba_size;
  uint32_t efba_pad_; //unused
  uint64_t efba_off;
};

#endif
