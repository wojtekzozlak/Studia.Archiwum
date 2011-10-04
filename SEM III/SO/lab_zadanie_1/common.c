#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "err.h"
#include "common.h"

void setup_child(const int *in, const int *out)
{
  close(in[1]);
  close(out[0]);
  dup2(in[0], 0);
  dup2(out[1], 1);
  return ;
}

void setup_parent(child_pipe *child, const int *in, const int *out)
{
  close(in[0]);
  close(out[1]);
  child->in = out[0];
  child->out = in[1];
  return ;
}

void destroy_child(child_pipe *child)
{
  command com;
  com.code = COM_STOP;
  checked_write(child->out, &com, sizeof(com));
  close(child->in);
  close(child->out);
  if(wait(0) == -1) syserr("wait failed");
  return ;
}

int* make_pipe()
{
  int *ret = malloc(2 * sizeof(int));
  if(pipe(ret) == -1) syserr("pipe failed");
  return ret;
}

int checked_write(int fd, const void *buf, size_t count)
{
  int ret = write(fd, buf, count);
  if(ret == -1) syserr("write failed");
  return ret;
}

int checked_read(int fd, void *buf, size_t count)
{
  int ret = read(fd, buf, count);
  if(ret == -1) syserr("read failed");
  return ret;
}
