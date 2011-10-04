#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "mesg.h"
#include "err.h"
 
int msg_qid;

void exit_server(int sig) 
{
  if (msgctl(msg_qid, IPC_RMID, 0) == -1)
    syserr("msgctl RMID");
  exit(0);
}

int main()
{
  Mesg mesg;
  int	n, filefd;
  char errmesg[256];

  if (signal(SIGINT,  exit_server) == SIG_ERR)
    syserr("signal");

  if ((msg_qid = msgget(MKEY, 0666 | IPC_CREAT | IPC_EXCL)) == -1)
    syserr("msgget");

  for(;;) {
    if ((n = msgrcv(msg_qid, &mesg, MAXMESGDATA, 1L, 0)) <= 0)
      syserr("msgrcv");
    mesg.mesg_data[n] = '\0';		/* null terminate filename */
    
    mesg.mesg_type = 2L;		/* send messages of this type */
    if ( (filefd = open(mesg.mesg_data, 0)) < 0) {
      sprintf(errmesg, "Can't open: %s\n", mesg.mesg_data);
      strcpy(mesg.mesg_data, errmesg);
      n = strlen(mesg.mesg_data);
	 
      if (msgsnd(msg_qid, (char *) &mesg, n, 0) != 0)
        syserr("msgsnd");
    } else {
      while ( (n = read(filefd, mesg.mesg_data, MAXMESGDATA)) > 0) 
	if (msgsnd(msg_qid, (char *) &mesg, n, 0) != 0)
	  syserr("msgsnd");
      close(filefd);
      if (n < 0)
	syserr("read");
    }
    
    /* Send a message with a length of 0 to signify the end.  */
    if (msgsnd(msg_qid, (char *) &mesg, 0, 0) != 0)
      syserr("msgsnd");
  }
}
