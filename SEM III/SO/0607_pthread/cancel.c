#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include "err.h"

#define losuj(min, max) (min)+(int) ((double) ((max)-(min)+1) * rand()                                                                      /  (RAND_MAX+1.0))
pthread_mutex_t sekcja;

void wyczysc (void *arg)
{
  int blad;
  printf ("Odblokowuje mutexa\n");
  if ((blad = pthread_mutex_unlock ((pthread_mutex_t *) arg)) != 0)
    syserr (blad, "unlock");
}

void *watek (void *data) 
{
  int stary;
  int blad;

  /* Enable cancellation in deferred mode */
  if ((blad = pthread_setcancelstate (PTHREAD_CANCEL_ENABLE, &stary)) != 0)
    syserr (blad, "setcancelstate");
  if ((blad = pthread_setcanceltype (PTHREAD_CANCEL_DEFERRED, &stary)) != 0)
    syserr (blad, "setcanceltype");
  
  for (;;) {
    sleep (losuj (1, 2));
    pthread_testcancel();
    pthread_cleanup_push (wyczysc, (void *) &sekcja); 
    if ( (blad = pthread_mutex_lock (&sekcja)) != 0)
      syserr (blad, "lock");
    printf ("Watek nr %s\n", (char *) data);
    sleep (losuj (1,3));
    pthread_testcancel();
    if ( (blad = pthread_mutex_unlock (&sekcja)) != 0)
      syserr (blad, "unlock");
    pthread_cleanup_pop (0); 
  }
}

int main (int argc, char *argv[])
{
  pthread_t th_a, th_b;
  pthread_attr_t attr;
  int blad;

  srand(time(0));

  if ((blad = pthread_mutex_init (&sekcja, 0)) != 0)
    syserr (blad, "mutex init"); 
  if ((blad = pthread_attr_init (&attr)) != 0)
    syserr (blad, "attr init");
  if ((blad = pthread_attr_setdetachstate (&attr,PTHREAD_CREATE_JOINABLE)) 
              != 0)
    syserr (blad, "setdetach");

  if ((blad = pthread_create (&th_a, &attr, watek, "a")) != 0)
    syserr (blad, "create");
   
  if ((blad = pthread_create (&th_b, &attr, watek, "b")) != 0)
    syserr (blad, "create");
    
  sleep (10);
  
  if ((blad = pthread_cancel (th_a)) != 0) 
    syserr (blad, "cancel");
  if ((blad = pthread_cancel (th_b)) != 0) 
    syserr (blad, "cancel");

  if ((blad = pthread_join (th_a, 0)) != 0)
    syserr (blad, "join");
  if ((blad = pthread_join (th_b, 0)) != 0)
    syserr (blad, "join");
  
  if ((blad = pthread_attr_destroy (&attr)) != 0)
    syserr (blad, "attrdestroy");

  if ((blad = pthread_mutex_destroy (&sekcja)) != 0)
    syserr (blad, "mutex destroy");
  return 0;
}
