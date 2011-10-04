#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include "common.h"
#include "err.h"

#ifdef DEBUG
const bool debug = true;
#else
const bool debug = false;
#endif

int pid;
int handler;
int id_rec, id_hand_in, id_hand_out;
long *res_msg;
bool send_msg;
bool the_end;

void resignation();

int safe_msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg)
/* Simple wrapper for the system `msgsnd` function, which terminates *
 * program execution if an error is encountered.                     */
{
  int result;
  if((result = msgsnd(msqid, msgp, msgsz, msgflg)) == -1)
  {
    if(the_end) resignation();
    else
      syserr("msgsnd");
  }
  return result;
}

void resignation()
/* Procedure sends a resignation message to the handler, cleans resources *
 * and terminates program.                                                */
{
  Message msg;
  the_end = false; // to prevent a infinite loop
  if(send_msg)
  {
    if(debug) fprintf(stderr, "client %d: sending a resignation message\n",
                      pid);
    set_message(&msg, handler, RESIGNATION, handler);
    safe_msgsnd(id_hand_in, &msg, MESSAGE_SIZE, 0);
  }
  free(res_msg);
  if(debug) fprintf(stderr, "client %d: interrupted by the user\n", pid);
  exit(0);
}

ssize_t safe_msgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp,
  int msgflg)
/* Simple wrapper for the system `msgrcv` function, which terminates *
 * program execution if an error is encountered.                     */
{
  ssize_t result;
  if((result = msgrcv(msqid, msgp, msgsz, msgtyp, msgflg)) == -1)
  {
    if(the_end) resignation();
    else
      syserr("msgrcv");
  }
  return result;
}

int open_client_queue(key_t key)
/* Simple wrapper for the system `msgget` function, which opens an IPC     *
 * queue in client mode using a [key] key and terminates program execution *
 * if an error is encountered.                                             */
{
  int id;
  if((id = msgget(key, IPC_EXCL)) == -1)
  {
    if(errno == ENOENT) fatal("Can't communicate with the server.");
    else syserr("msgget");
    return -1;
  }
  else return id;
}

int get_handler()
/* Function asks the server for a handler id and returns the answer. */
{
  Message msg;
  set_message(&msg, RECEPTION, HANDLER_REQ, 0);

  if(debug) fprintf(stderr, "client %d: requesting a handler\n", pid);
  safe_msgsnd(id_rec, &msg, MESSAGE_SIZE, 0);
  if(debug) fprintf(stderr, "client %d: waiting for the handler\n", pid);
  safe_msgrcv(id_hand_out, &msg, MESSAGE_SIZE, ID_REPO, 0);
  if(msg.comm != HANDLER_OFFER)
    fatal_msg_expected("HANDLER_OFFER", msg.comm);
  if(debug) fprintf(stderr, "client %d: the %ld handler received\n", pid,
                    msg.value);
  return msg.value;
}

void shut_down(int signal)
/* Procedure handles the SIGINT singal by changing the `the_end` flag,   *
 * what triggers program termination. We can't end program here, because *
 * `msgsnd` is not safe while signal handling.                           */
{
  the_end = true;
  return ;
}

void init(int argc, char *argv[], int *R, int *T, int *count)
{
/* Procedure inits client.                                                   *
 * It opens IPC queues, reads a start parameters and sets up SIGINT handler. *
 *                                                                           *
 * MUTABLE PARAMETERS: R, T, count                                           */
  int i;
  if(argc < 3)
    fatal("to few arguments");
  the_end = false;

  id_rec = open_client_queue(MKEY_RECEPTION);
  id_hand_in = open_client_queue(MKEY_HANDLER_IN);
  id_hand_out = open_client_queue(MKEY_HANDLER_OUT);

  *R = atoi(argv[1]);
  *T = atoi(argv[2]);
  *count = argc - 3;
  res_msg = malloc((*count + 2) * sizeof(long));
  res_msg[1] = RESOURCES;
  for(i = 0; i < *count; i++)
    res_msg[2 + i] = atoi(argv[3 + i]);

  send_msg = false;
  if(signal(SIGINT, shut_down) == SIG_ERR)
    syserr("signal");
  return ;
}

int main(int argc, char *argv[]){
  int i;
  int R, T, count;
  Message msg;
  pid = getpid();
  handler = -1;

  if(debug) fprintf(stderr, "client %d: initialization\n", pid);
  init(argc, argv, &R, &T, &count);

  for(i = 0; i < R; i++)
  {
    if(the_end) resignation();
    send_msg = true;
    handler = get_handler();

    if(the_end) resignation();
    if(debug) fprintf(stderr, "client %d: sending a request for resources\n",
                      pid);
    set_message(&msg, handler, WANT_RESOURCES, count);
    safe_msgsnd(id_hand_in, &msg, MESSAGE_SIZE, 0);
    res_msg[0] = handler;
    safe_msgsnd(id_hand_in, res_msg, sizeof(long) * (count + 1), 0);

    if(the_end) resignation(); 
    if(debug) fprintf(stderr, "client %d: waiting for resources\n", pid);
    safe_msgrcv(id_hand_out, &msg, MESSAGE_SIZE, handler, 0);
    if(msg.comm == CANT_SATISFY)
      fatal("Server can not complete my request");
    else if(msg.comm != RESOURCES_AQUIRED)
      fatal_msg_expected("RESOURCES_AQUIRED or CANT_SATISFY", msg.comm);

    if(the_end) resignation();
    if(debug) fprintf(stderr, "client %d: working\n", pid);
    sleep(T);

    if(the_end) resignation();
    if(debug) fprintf(stderr, "client %d: sending the end-of-work message\n",
                      pid);
    set_message(&msg, handler, END_OF_WORK, 0);
    safe_msgsnd(id_hand_in, &msg, MESSAGE_SIZE, 0);
    send_msg = false;
  }  

  free(res_msg);
  return 0;
}
