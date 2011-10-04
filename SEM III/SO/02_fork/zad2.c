#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "err.h"

int main()
{
  pid_t pid;
  bool forkFlag = true;
  int i;
  int p=5;

  printf("Moj PID = %d\n", getpid());

  for(i=1; i<p; i++)
  {
    if(forkFlag){
      switch(pid = fork())
      {
        case -1: /* nie wyszlo */
          syserr("Error in fork\n");
          break;

        case 0: /* proces potomny */
          printf("Jestem procesem potomnym nr %d,o PID = %d\n", i, getpid());
          if(i == p-1) return 0;
          break;

        default: /* proces macierzysty */
          forkFlag = false;
          break;

      }
    } else break;
  }

  if (wait(0) == -1)
    syserr("Error in wait\n");

    return 0;


}
