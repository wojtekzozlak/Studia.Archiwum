#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <pthread.h>
#include "err.h"

#define ILERAZY      3
#define ILEWATKOW    2
#define POSTOJ       4
#define ETIMEDOUT    110
/* brakuje tej definicji w standardowych plikach naglowkowych */
int zajete = 0;
pthread_mutex_t sekcja;
pthread_cond_t war;


void *watek (void *data) 
{
  int i;
  int blad;
  struct timeval now;
  struct timespec timeout;

  printf ("Watek nr %d\n", *(int *)data);
  sleep (POSTOJ);
  
  for (i = 0; i < ILERAZY; i++) {
    if ((blad = pthread_mutex_lock(&sekcja)) != 0)
      syserr (blad, "lock1");
    while (zajete) {
      printf ("Watek nr %d czeka na dostep do sekcji\n", * (int *) data);
      gettimeofday(&now, 0);
      timeout.tv_sec = now.tv_sec + 2;
      timeout.tv_nsec = now.tv_usec * 1000; 
      if ((blad = pthread_cond_timedwait(&war, &sekcja, &timeout)) != 0) {
        if (blad == ETIMEDOUT)
	  printf ("TIMEOUT!\n");
        else
          syserr (blad, "timedwait");	
      }
    }			
    zajete = 1;	
    if ((blad = pthread_mutex_unlock(&sekcja)) != 0)
      syserr (blad, "unlock1");

    printf ("Watek %d jest w sekcji krytycznej\n", *(int *)data);
    sleep (POSTOJ);

    if ((blad = pthread_mutex_lock(&sekcja)) != 0)
      syserr (blad, "lock2");
    zajete = 0;
    if ((blad = pthread_cond_signal(&war)) != 0)
      syserr (blad, "signal");
    if ((blad = pthread_mutex_unlock(&sekcja)) != 0)
      syserr (blad, "unlock2");
  }
  free (data);
  return 0;  
}

int main (int argc, char *argv[])
{
  pthread_t th[ILEWATKOW];
  pthread_attr_t attr;
  int i;
  int blad;

  if ((blad = pthread_mutex_init (&sekcja, 0)) != 0)
    syserr (blad, "mutex init");
  if ((blad = pthread_cond_init (&war, 0)) != 0)
    syserr (blad, "cond init"); 
  if ((blad = pthread_attr_init (&attr)) != 0)
    syserr (blad, "attr init");
  if ((blad = pthread_attr_setdetachstate (&attr,PTHREAD_CREATE_JOINABLE)) 
       != 0)
    syserr (blad, "setdetach");
  
  for (i = 0; i < ILEWATKOW; i++) {
    int *nr = (int *) malloc (sizeof(int));
    if (nr == NULL)
      syserr (blad, "malloc");
    *nr = i;
    if ((blad = pthread_create (th + i, &attr, watek, (void *)nr)) != 0)
      syserr (blad, "create");
  }
  
  for (i = 0; i < ILEWATKOW; i++)
    if ((blad = pthread_join (th[i], 0)) != 0)
      syserr (blad, "join");
  
  if ((blad = pthread_attr_destroy (&attr)) != 0)
    syserr (blad, "attrdestroy");
  if ((blad = pthread_cond_destroy (&war)) != 0)
    syserr (blad, "cond destroy"); 
  if ((blad = pthread_mutex_destroy (&sekcja)) != 0)
    syserr (blad, "mutex destroy"); 
  return 0;
}
