#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "common.h"
#include "err.h"

#define LINE_SIZE 128
#define BUF_SIZE 32

#ifdef QUIET
const int prompt = 0;
#else
const int prompt = 1;
#endif

int check_command(const char *c)
/* Function checks if [c] contains proper command string. If so - it returns *
 * corresponding code. If not so - it returns COM_UNKNOWN.                   */
{
  if(strcmp(c, "add") == 0) return COM_ADD;
  if(strcmp(c, "find") == 0) return COM_FIND;
  if(strcmp(c, "walk") == 0) return COM_WALK;
  if(strcmp(c, "stop") == 0) return COM_STOP;
  if(strcmp(c, "help") == 0) return COM_HELP;
  return COM_UNKNOWN;
}

void help()
/* Procedure prints help info into standard output */
{
  printf("Available commands:\n\
\tadd\n\
\tfind\n\
\twalk\n\
\tstop\n");
}

void add(child_pipe *root, int value)
/* If [root] is not set (in descriptor is equal to -1) program forks,        *
 * executes `vertex` program with [value] as argument and sets pipes for     *
 * communication. Otherwise it sends ADD command through pipe and awaits     *
 * for confirmation from child process                                       *
 *                                                                           *
 * MUTABLE PARAMETERS: root                                                  */
{
  char arg[BUF_SIZE];
  command com;
  int result;
  if(root->in == -1)
  {
    sprintf(arg, "%d", value);
    int *in = make_pipe(), *out = make_pipe();
    switch(fork())
    {
      case -1:
        syserr("fork failed");
        break;
      case 0:
        setup_child(in, out);
        if(execl("./vertex", "vertex", arg, NULL) == -1)
          syserr("exec failed");
        break;
      default:
        setup_parent(root, in, out);
        break;
    }
    free(in);
    free(out);
  }
  else
  {
    com.code = COM_ADD;
    com.arg = value;
    checked_write(root->out, &com, sizeof(com));
    checked_read(root->in, &result, sizeof(result));
  }
  return ;
}

void find(const child_pipe *root, int value)
/* Procedure sends `FIND [value]` command using [root] for communication, *
 * awaits for answer and prints it into standard output.                  *
 * If [root] is not set (have empty descriptor) procedure prints '0'.     */
{
  int count = 0;
  command com;
  if(root->in != -1)
  {
    com.code = COM_FIND;
    com.arg = value;
    checked_write(root->out, &com, sizeof(com));
    checked_read(root->in, &count, sizeof(count));
  }
  printf("%d\n", count);
}

void walk(const child_pipe *root)
/* Procedure sends WALK command using [root] for communication,  *
 * awaits for answer and prints it into standard output.         *
 * If [root] is not set (have empty descriptor) procedure prints *
 * `(empty_set)`.                                                */
{
  command com;
  int count, mp, val;
  int i, j;
  com.code = COM_WALK;
  if(root->in != -1)
  {
    checked_write(root->out, &com, sizeof(com));
    checked_read(root->in, &count, sizeof(count));
    for(i = 0; i < count; i++)
    {
      val = 1;
      checked_write(root->out, &val, sizeof(val));
      checked_read(root->in, &mp, sizeof(mp));
      checked_read(root->in, &val, sizeof(val));
      for(j = 0; j < mp; j++) printf("%d ", val);
    }
    printf("\n");
  }
  else printf("(empty_set)\n");
}

void stop(child_pipe *root)
/* If [root] is set procedure initializes corresponding child process     *
 * termination and cleanup (see `destroy_child` for more info).           *
 * Then terminates program.                                               *
 *                                                                        *
 * MUTABLE PARAMETERS: root                                               */
{
  if(root->in != -1)
    destroy_child(root);
  exit(0);
}

int main()
{
  child_pipe root;
  char *line;
  char command[BUF_SIZE];
  int com_code, arg_pos;
  size_t len = 0;
  int com_arg;
  root.in = -1;

  if(prompt) printf("?: ");
  while(getline(&line, &len, stdin) != -1)
  {
    sscanf(line, "%s", command);
    com_code = check_command(command);
    arg_pos = strlen(command); /* Offset used in argument parsing. */

    switch(com_code)
    {
      case COM_ADD:
        if(sscanf(line + arg_pos, "%d", &com_arg) > 0)
        {
          add(&root, com_arg);
          if(prompt) printf("value added\n");
        }
        else com_code = COM_UNKNOWN;
        break;
      case COM_FIND:
        if(sscanf(line + arg_pos, "%d", &com_arg) > 0)
        {
          if(prompt) printf("count: ");
          find(&root, com_arg);
        }
        else com_code = COM_UNKNOWN;
        break;
      case COM_WALK:
        if(prompt) printf("values: ");
        walk(&root);
        break;
      case COM_STOP:
        stop(&root);
        break;
      case COM_HELP:
        help();
        break;
      default:
        break;
    }
    if(prompt && com_code == COM_UNKNOWN)
      printf("Error: Bad input, try \"help\" to obtain more info.\n");
    if(prompt) printf("?: ");
  }
  return 0;
}
