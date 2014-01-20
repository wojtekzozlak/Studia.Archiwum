#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>    /* open */
#include <unistd.h>   /* exit */
#include <sys/ioctl.h>    /* ioctl */
#include <stdint.h>

#include "e2fs_frag.h"

int main() {
  int fd;
  fd = open("/dev/zero_bdev1", 0);
  if (fd <= 0) {
    printf("can't open\n");
    exit(1);
  }

  struct zero_bdev_config_arg config;
  config.zbc_block_size = 4096;
  config.zbc_num_blocks = 70;

  ioctl(fd, ZERO_BDEV_CONFIGURE, &config);

  close(fd);
  return 0;
}
