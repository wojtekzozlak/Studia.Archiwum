#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "err.h"

int main ()
{
  pid_t pid;
  
   /* wypisuje identyfikator procesu */
  printf("Moj PID = %d\n", getpid());

   /* tworzy nowy proces */
  switch (pid = fork()) {
    case -1: /* blad */
      syserr("Error in fork\n");

    case 0: /* proces potomny */
 
      printf("Jestem procesem potomnym. Moj PID = %d\n", getpid());
      printf("Jestem procesem potomnym. Wartosc przekazana przez fork() =\
 %d\n", pid);
      
      return 0;
    
    default: /* proces macierzysty */

      printf("Jestem procesem macierzystym. Moj PID = %d\n", getpid());
      printf("Jestem procesem macierzystym. Wartosc przekazana przez fork() =\
 %d\n", pid);
       
       /* czeka na zakonczenie procesu potomnego */

      if (wait(0) == -1) 
	syserr("Error in wait\n");

      return 0;
  } /*switch*/
}

