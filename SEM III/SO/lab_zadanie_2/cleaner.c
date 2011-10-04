#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <pthread.h>
#include <errno.h>
#include <stdio.h>
#include "common.h"
#include "err.h"

int id_rec, id_gravedigger, id_hand_in, id_hand_out;

int open_server_queue(key_t key)
{
  int id;
  if((id = msgget(key, 0666 | IPC_CREAT)) == -1)
    syserr("msgget");
  return id;
}

int main(){
  struct msqid_ds info;

  id_rec = open_server_queue(MKEY_RECEPTION);
  id_gravedigger = open_server_queue(MKEY_GRAVEDIGGER);
  id_hand_in = open_server_queue(MKEY_HANDLER_IN);
  id_hand_out = open_server_queue(MKEY_HANDLER_OUT);

  msgctl(id_rec, IPC_RMID, &info);
  msgctl(id_gravedigger, IPC_RMID, &info);
  msgctl(id_hand_in, IPC_RMID, &info);
  msgctl(id_hand_out, IPC_RMID, &info);

  return 0;
}
