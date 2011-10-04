#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "err.h"

char message[] = "Hello from your parent!";

int main()
{
  int pipe_dsc[2];
  char pipe_read_dsc_str[10];
  
  if (pipe (pipe_dsc) == -1) syserr("Error in pipe\n");
  
  switch (fork()) {
    case -1:
      syserr("Error in fork\n");
    
    case 0: 
      if (close (pipe_dsc[1]) == -1) syserr("Error in close (pipe_dsc[1])\n");
      
      sprintf(pipe_read_dsc_str, "%d", pipe_dsc[0]);
      execl("./child_pipe", "child_pipe", pipe_read_dsc_str, (char *) 0);
      syserr("Error in execl\n");
      
    default:
      if (close (pipe_dsc[0]) == -1) syserr("Error in close (pipe_dsc[0])\n");
      
      if (write (pipe_dsc[1], message, sizeof(message)) == -1)
	syserr("Error in close write\n");
      
      if (wait (0) == -1)
	syserr("Error in wait\n");
      
      exit (0);
  } /* switch */
}
