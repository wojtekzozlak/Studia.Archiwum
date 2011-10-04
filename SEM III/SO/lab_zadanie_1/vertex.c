#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "common.h"
#include "err.h"

struct vertex_struct {
  int value, count;
  child_pipe left, right;
};

typedef struct vertex_struct vertex;

void vertex_init(vertex *v, int value)
/* Procedure initializes [v] with no children, count equal to one and  *
 * [value] as value.                                                   *
 *                                                                     *
 * MUTABLE PARAMETERS: v                                               */
{
  v->value = value;
  v->count = 1;
  if(v->left.in != -1){
    close(v->left.in);
    close(v->left.out);
    v->left.in = -1;
  }
  if(v->right.in != -1){
    close(v->right.in);
    close(v->right.out);
    v->right.in = -1;
  }
  return ;
}

void add_fork(child_pipe *child, int value, vertex *v)
/* Procedure forks vertex. Child process will be initialized with [value] as *
 * value, count equal to one and pipes duped into {0, 1} descriptors.        *
 * Parent process populates [child] structure with analogical descriptors    *
 * providing way to communication. Then sends confirmation signal into       *
 * output.                                                                   *
 *                                                                           *
 * MUTABLE PARAMETERS: v, child                                              */
{
  int result = 1;
  int *in = make_pipe(), *out = make_pipe();
  switch(fork())
  {
    case -1:
      syserr("fork failed");
      break;
    case 0:
      setup_child(in, out);
      vertex_init(v, value);
      break;
    default:
      setup_parent(child, in, out);
      checked_write(1, &result, sizeof(result));
      break;
  }
  free(in);
  free(out);
  return ;
}

void add(vertex *v, command com)
/* Procedure                                                             *
 *  - Increments counter in [v] if [com.arg] is equal to [v->value]      *
 *  or                                                                   *
 *  - Sends [com] into suitable (right if [com.arg] > [v->value], left   *
 *    otherwise)                                                         *
 *  and then prints `1` as confirmation message into output.             *
 *                                                                       *
 * MUTABLE PARAMETERS: v                                                 */
{
  int result = 1;
  child_pipe *child;
  if(com.arg == v->value)
  {
    v->count++;
    checked_write(1, &result, sizeof(result));
    return ;
  }
  child = (com.arg > v->value ? &v->right : &v->left);
  if(child->in == -1) add_fork(child, com.arg, v);
  else
  {
    checked_write(child->out, &com, sizeof(com));
    checked_read(child->in, &result, sizeof(result));
    checked_write(1, &result, sizeof(result));
  }
  return ;
}

void find(vertex *v, command com)
/* Procedure checks if BST cointains [com.arg] value.                    *
 *                                                                       *
 * If current vertex contains [com.arg] value it returns [v->counter]    *
 * into output. If not so, it sends a [com] (FIND) command into suitable *
 * (right if [com.arg] > [v->value], left otherwise) child, awaits for   *
 * answer and prints it into output. If there is no proper child,        *
 * procedure prints `0`.                                                 */
{
  int result;
  child_pipe *child;
  if(com.arg == v->value)
  {
    checked_write(1, &v->count, sizeof(v->count));
  }
  else
  {
    child = (com.arg > v->value ? &v->right : &v->left);
    if(child->in == -1)
    {
      result = 0;
      checked_write(1, &result, sizeof(result));
    }
    else
    {
      checked_write(child->out, &com, sizeof(com));
      checked_read(child->in, &result, sizeof(result));
      checked_write(1, &result, sizeof(result));
    }
  }
  return ;
}

void walk_ask_childs_count(const vertex *v, int *lcount, int *rcount)
/* Procedure sends a WALK command to the children, then reads sizes of        *
 * subtrees and puts them in [lcount] and [rcount].                           *
 *                                                                            *
 * MUTABLE PARAMETERS: lcount, rcount                                         */
{
  command com;
  com.code = COM_WALK;
  if(v->left.in != -1) checked_write(v->left.out, &com, sizeof(com));
  if(v->right.in != -1) checked_write(v->right.out, &com, sizeof(com));
  /* Reading from the first child before sending request to the second one  *
   * will freeze the program, so the calculation would not be parallel.     *
   * Although, it is not very lucrative in this particular case, it         *
   * would be for more complex vertex operations.                           */
  if(v->left.in != -1) checked_read(v->left.in, lcount, sizeof(int));
  if(v->right.in != -1) checked_read(v->right.in, rcount, sizeof(int));
  return ;
}

void walk_ask_child_val(const child_pipe *child)
/* Procedure awaits for signal from parent, than asks [child] for one value *
 * and sends it to the output. Procedure assuming that process is currently *
 * WALKing through tree values.                                             */
{
  int count, val = 1, signal;
  checked_read(0, &signal, sizeof(signal));
  checked_write(child->out, &val, sizeof(val));
  checked_read(child->in, &count, sizeof(count));
  checked_read(child->in, &val, sizeof(val));
  checked_write(1, &count, sizeof(count));
  checked_write(1, &val, sizeof(val));
  return ;
}

void walk(const vertex *v)
/* Procedure calculates size of the tree, sends its into output, and than    *
 * sends values from the tree in infix order. Every value transmition have   *
 * to be invoked by parent.                                                  */
{
  int lcount = 0, rcount = 0, count, i, signal;
  walk_ask_childs_count(v, &lcount, &rcount);
  count = lcount + rcount + 1;
  checked_write(1, &count, sizeof(count));
  for(i = 0; i < lcount; i++) walk_ask_child_val(&v->left);
  checked_read(0, &signal, sizeof(signal));
  checked_write(1, &v->count, sizeof(v->count));
  checked_write(1, &v->value, sizeof(v->value));
  for(i = 0; i < rcount; i++) walk_ask_child_val(&v->right);
  return ;
}

int main(int argc, char *argv[])
{
  vertex v;
  command com;
  v.left.in = -1;
  v.right.in = -1; 
 
  vertex_init(&v, 0);
  sscanf(argv[1], "%d", &v.value);

  while(checked_read(0, &com, sizeof(com)))
  {
    switch(com.code)
    {
      case COM_ADD:
        add(&v, com);
        break;
      case COM_FIND:
        find(&v, com);
        break;
      case COM_WALK:
        walk(&v);
        break;
      case COM_STOP:
        if(v.left.in != -1) destroy_child(&v.left);
        if(v.right.in != -1) destroy_child(&v.right);
        close(0);
        close(1);
        exit(0);
        break;
      default:
        fatal("code unknown");
        break;
    }
  }
  return 0;
}
