#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include "err.h"

/* parse - dzieli polecenie z buf na pojedyncze slowa i umieszcza je w args */
void parse (char buf[], char *args[])
{
  int i = 0, j = 0;
  
  while (buf[i] != '\0') {

     /* omija biale znaki */
    for (; buf[i] == ' ' || buf[i] == '\t'; buf[i++] = '\0');

     /* zapamietuje argument */
    args[j] = buf + i;
    j++;
    
     /* omija argument */
    for (; buf[i] != '\0' && buf[i] != ' ' && buf[i] != '\t'; i++);
  } 

  args[j] = 0;
} /* parse */

/* execute - tworzy proces potomny, ktory wykonuje polecenie */
void execute(char *args[])
{
  int pid;
  
  switch(pid = fork()) {
    case -1: /* blad */
      syserr("Error in fork\n");

    case 0: /* proces potomny */
      execvp(args[0], args);
      syserr("Error in execlp %s\n", args[0]);

    default: /* proces macierzysty */
      while (wait(0) != pid);  
  }
} /* execute */

int main()
{
  char buf[1024], *args[64];
  
  for (;;) {

     /* prosi o polecenia i wczytuje je na buf */
    printf("Polecenie: ");
    
    if  (fgets(buf, sizeof(buf), stdin) == 0) {
      printf("\n");
      return 0;
    }
    
    if (strlen(buf) == 0) 
      continue;
     /* wstawia znak konca napisu (zamiast znaku nowego wiersza) */
    buf[strlen(buf) - 1] = '\0';

     /* analizuje polecenie ... */    
    parse(buf, args);

     /* ... i je wykonuje */
    execute(args);
  } 
} /* main */







