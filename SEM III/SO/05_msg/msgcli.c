#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include "mesg.h"
#include "err.h"

int main()
{
  Mesg mesg;
  int id, n;
   
  if ( (id = msgget(MKEY, 0)) == -1)
    syserr("msgget");

  printf("File name: ");
  if (fgets(mesg.mesg_data, MAXMESGDATA, stdin) == 0)
    syserr("fgets");

  n = strlen(mesg.mesg_data);
  if (mesg.mesg_data[n-1] == '\n')
    n--;				/* ignore the newline from fgets() */
  mesg.mesg_data[n] = '\0';		/* overwrite newline at end */
  mesg.mesg_type = 1L;			/* send messages of this type */
  if (msgsnd(id, (char *) &mesg, n, 0) != 0)
    syserr("msgsnd");
   
  while( (n = msgrcv(id, &mesg, MAXMESGDATA, 2L, 0)) > 0)
    if (write(1, mesg.mesg_data, n) != n)
      syserr("write");
   
  if (n < 0)
    syserr("msgrcv");
  return 0;
}
