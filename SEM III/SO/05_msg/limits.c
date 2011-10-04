#include <unistd.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <errno.h>
#include <stdio.h>
#include "err.h"	

#define	MAX_DATA 64*1024
#define	MAX_NMESG 4096
#define	MAX_NIDS 4096
int max_mesg;

struct mymesg {
  long	type;
  char	data[MAX_DATA];
} mesg;

int main(int argc, char **argv)
{
   int i, j, msqid, qid[MAX_NIDS];

   /* first try and determine maximum amount of data we can send */
   if ((msqid = msgget(IPC_PRIVATE, 0666 | IPC_CREAT)) == -1)
     syserr("msgget1");
   mesg.type = 1;
   for (i = MAX_DATA; i > 0; i -= 4) {
      if (msgsnd(msqid, &mesg, i, 0) == 0) {
	 printf("maximum amount of data per message = %d\n", i);
	 max_mesg = i;
	 break;
      }
      if (errno != EINVAL)
	syserr("msgsnd, length %d", i);
   }
   if (i == 0)
     syserr("i == 0");
   if (msgctl(msqid, IPC_RMID, NULL) == -1)
     syserr("msgctl1");
   
   /* see how many messages of varying size can be put onto a queue */
   mesg.type = 1;
   for (i = 8; i <= max_mesg; i *= 2) {
      if ((msqid = msgget(IPC_PRIVATE, 0666 | IPC_CREAT)) == -1)
        syserr("msgget2");
      for (j = 0; j < MAX_NMESG; j++) {
	 if (msgsnd(msqid, &mesg, i, IPC_NOWAIT) != 0) {
	    if (errno == EAGAIN)
	      break;
	    syserr("msgsnd, i = %d, j = %d", i, j);
	    break;
	 }
      }
      printf("%4d %4d-byte messages were placed onto queue,", j, i);
      printf(" %d bytes total\n", i*j);
      if (msgctl(msqid, IPC_RMID, NULL) == -1)
        syserr("msgctl2");
   }
   
   /* see how many identifiers we can "open" */
   mesg.type = 1;
   for (i = 0; i <= MAX_NIDS; i++) {
      if ((qid[i] = msgget(IPC_PRIVATE, 0666 | IPC_CREAT)) == -1) {
	 printf("%d identifiers open at once\n", i);
	 break;
      }
   }
   for (j = 0; j < i; j++)
     if (msgctl(qid[j], IPC_RMID, NULL) == -1)
       syserr("msgctl3 j = %d", j);
   exit(0);
}
