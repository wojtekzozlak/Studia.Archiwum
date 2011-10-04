#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>

#include "err.h"

#define MAX_SIZE 80

int msgget_wrapper(key_t key) {
  int opperm, opperm_flag, flags;

  printf( "\nEnter attributed permissions in octal = ");
  scanf(  "%o", &opperm );

  printf( "\nEnter corresponding number to set desired flags:\n");
  printf( "0 --> No flags \n");
  printf( "1 --> IPC_CREAT \n");
  printf( "2 --> IPC_EXCL \n");
  printf( "3 --> IPC_CREAT and IPC_EXCL \n");
  printf( "Flags = ");
  scanf(  "%d", &flags );

  printf( "\nkey = 0x%x, opperm = 0%o, flags = 0%o\n", key, opperm, flags );

  switch ( flags ) {
  case 0:
    opperm_flag = opperm | 0;
    break;
  case 1:
    opperm_flag = opperm | IPC_CREAT;
    break;
  case 2:
    opperm_flag = opperm | IPC_EXCL;
    break;
  case 3:
    opperm_flag = opperm | IPC_EXCL | IPC_CREAT;
    break;
  }

  /*********************************************************/

  return msgget( key, opperm_flag );

  /*********************************************************/
}


struct mymsgbuf {
  long mtype;
  char mtext[MAX_SIZE];
};

void send_message(int qid, struct mymsgbuf *qbuf, long type, char *text)
{
  printf("Sending a message ...\n");
  qbuf->mtype = type;
  strncpy(qbuf->mtext, text, MAX_SIZE);
  qbuf->mtext[MAX_SIZE - 1] = '\0';    /* to null-terminate too long strings */

  if((msgsnd(qid, (struct msgbuf *)qbuf, strlen(qbuf->mtext) + 1, 0)) ==-1) {
    syserr("msgsnd in send_message");
    exit(1);
  }
}

void read_message(int qid, struct mymsgbuf *qbuf, long type)
{
  printf("Reading a message ...\n");
  qbuf->mtype = type;
  if (msgrcv(qid, (struct msgbuf *)qbuf, MAX_SIZE, type, 0) == -1)
    syserr("msgrcv in read_message");

  printf("Type: %ld Text: %s\n", qbuf->mtype, qbuf->mtext);
}

void remove_queue(int qid)
{
  if (msgctl(qid, IPC_RMID, 0) == -1)
    syserr("msgctl in remove_queue");
}

void change_queue_mode(int qid, unsigned short opperm)
{
  struct msqid_ds myq_ds;

  if (msgctl(qid, IPC_STAT, &myq_ds) == -1)	/* Get current info */ 
    syserr("msgctl1 in change_queue_mode");
  myq_ds.msg_perm.mode = opperm;
  if (msgctl(qid, IPC_SET, &myq_ds) == -1)	/* Update the mode */
    syserr("msgctl2 in change_queue_mode");
}

int main(int argc, char *argv[])
{
  struct mymsgbuf qbuf;

  char key_id[16];
  key_t key;
  int msgid, type;
  unsigned short opperm;
  char cmd[MAX_SIZE] = { 0 };

  printf ( "Message queues present in the system\n" );
  system("ipcs -q");

  printf( "\nEnter the desired key in hex (form: 0x0807633d, 0x0 for IPC_PRIVATE)\n      or queue's id in decimal: ");
  scanf(  "%15s", key_id);
  if ( sscanf(  key_id, "0x%x", &key ) == 0 ) { // key_id represents an ID
      sscanf( key_id, "%d", &msgid );
      printf( "\nmsgid = %d", msgid );
  } else { // key_id is a key, otherwise
      msgid = msgget_wrapper( key );	
      if ( msgid == -1 )
         syserr("msgget");
  }
  printf( "\nmsgid = %d\n", msgid );

  while (1) {
   fprintf(stderr, "\nChoose an action:\n");
   fprintf(stderr, "    (s)end <type> <messagetext>\n");
   fprintf(stderr, "    (r)ecv <type>\n");
   fprintf(stderr, "    (d)elete\n");
   fprintf(stderr, "    (m)ode <octal mode>\n");
   fprintf(stderr, "    (q)uit the application\n");
   scanf("%s", cmd);

   switch(tolower(cmd[0])) {
      case 's': 
         scanf("%d", &type);
         fgets(cmd, MAX_SIZE-1, stdin);
         send_message(msgid, &qbuf, type, cmd);
         break;
      case 'r': 
         scanf("%d", &type);
         read_message(msgid, &qbuf, type);
         break;
      case 'd': 
         remove_queue(msgid);
         break;
      case 'm':
         scanf("%ho", &opperm);
         change_queue_mode(msgid, opperm);
         break;
      case 'q': 
         return 0;
   }
  }
  return 0 ;
}
