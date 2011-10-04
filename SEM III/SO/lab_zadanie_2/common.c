#include <stdio.h>
#include "err.h"
#include "common.h"

void set_message(Message *msg, long mtype, long comm, long value)
{
  msg->mtype = mtype;
  msg->comm = comm;
  msg->value = value;
}

int compare_long(const void *pa, const void *pb)
{
  long a = *(const long*)pa;
  long b = *(const long*)pb;
  return (a - b);
}

void fatal_msg_expected(const char *type, long comm)
{
  char msg[1000];
  sprintf(msg, "Unexpected message with code %ld (%s expected).", comm, type);
  fatal(msg);
  return ;
}
