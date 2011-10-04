#ifndef _COMMON_H_
#define _COMMON_H_

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "err.h"

#define COM_UNKNOWN -1
#define COM_HELP 0
#define COM_ADD 1
#define COM_FIND 2
#define COM_WALK 3
#define COM_STOP 4

struct command_struct {
  int code;
  int arg;
};

struct child_pipe_struct
{
  int in; /* -1 for an empty descriptor */
  int out;
};

typedef struct command_struct command;
typedef struct child_pipe_struct child_pipe;

void setup_child(const int *in, const int *out);
/* Procedure sets input and output (0, 1) descriptors for a child process. */

void setup_parent(child_pipe *child, const int *in, const int *out);
/* Procedure sets input and output descriptors in the [child] structure *
 * for parent process.                                                  */

void destroy_child(child_pipe *child);
/* Procedure sends a STOP command to the [child], close [child] descriptors *
 * and awaits for termination of a child.                                   */

int* make_pipe();
/* Function returns pointer to a dynamically allocated array with a pipe *
 * descriptors.                                                          */

int checked_write(int fd, const void *buf, size_t count);
/* Function is a system write() function decorated with return value check. *
 * For more info check `man 2 write` page.                                  */

int checked_read(int fd, void *buf, size_t count);
/* Function is a system read() function decorated with return value check. *
 * For more info check `man 2 read` page.                                  */

#endif
