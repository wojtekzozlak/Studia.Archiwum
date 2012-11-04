#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>    /* open */
#include <unistd.h>   /* exit */
#include <sys/ioctl.h>    /* ioctl */
#include <stdint.h>
#include <errno.h>

#include "e2fs_frag.h"

int main(int argc, char *argv[]) {
  int fd;
  fd = open(argv[1], O_CREAT | O_RDWR);
  if (fd <= 0) {
    printf("can't open\n");
    exit(1);
  }

  struct ext2_fake_b_alloc_arg config;
  config.efba_size = 10000; // 1kB
  config.efba_off = 15000; // 1.5kB

  int ret = ioctl(fd, EXT2_FAKE_B_ALLOC, &config);
  printf("ret: %d\n", ret);
  if(ret < 0) {
    printf("%s\n", strerror(errno));
  }

  close(fd);
  return 0;
}
