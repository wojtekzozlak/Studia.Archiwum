#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include "err.h"

#define ILEWATKOW 3
#define POSTOJ    3

int ile = 0;

void *watek (void *data) 
{
  pid_t pid = getpid();

  ile++; /* Odwolanie do zmiennej dzielonej ktore moze prowadzic do bledow ! */
  printf ("Jestem %d watek. Moj PID=%d\n", ile, pid);
  return (void *) ile;
}

int main (int argc, char *argv[])
{
  pthread_t th[ILEWATKOW];
  pthread_attr_t attr;
  int i, wynik;
  int blad;
  pid_t pid = getpid();

  printf("Proces %d tworzy watki\n", pid);
  
  if ((blad = pthread_attr_init(&attr)) != 0 )
    syserr(blad, "attrinit");
  
  if ((blad = pthread_attr_setdetachstate(&attr, 
	   argc > 1 ? PTHREAD_CREATE_DETACHED : PTHREAD_CREATE_JOINABLE)) != 0)
    syserr(blad, "setdetach");
  
  for (i = 0; i < ILEWATKOW; i++)
    if ((blad = pthread_create(&th[i], &attr, watek, 0)) != 0)
      syserr(blad, "create");
  
  if (argc > 1) {
    sleep(POSTOJ);
    printf("KONIEC\n");
  } else { 
    printf ("Proces %d czekam na zakonczenie watkow\n", pid);
    for (i = 0; i < ILEWATKOW; i++) {
      if ((blad = pthread_join(th[i], (void **) &wynik)) != 0)
	syserr(blad, "join");
      printf("Kod powrotu %d\n", wynik);
    }
  }

  if ((blad = pthread_attr_destroy (&attr)) != 0)
    syserr(blad, "attrdestroy");
  return 0;
}
