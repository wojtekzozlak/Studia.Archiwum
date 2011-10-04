#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define LINE_SIZE 100

void *
handle_connection (void *s_ptr)
{
  int ret, s;
  size_t len;
  char line[LINE_SIZE + 1], peername[LINE_SIZE + 1], peeraddr[LINE_SIZE + 1];
  struct sockaddr_in addr;

  s = *(int *) s_ptr;
  free (s_ptr);

  len = sizeof (addr);
  ret = getpeername (s, (struct sockaddr *) &addr, &len);
  /* mowi kto jest po drugiej stronie gniazda (adres)  */
  if (ret == -1)
    {
      perror ("getsockname");
      exit (1);
    }
  inet_ntop (AF_INET, &addr.sin_addr, peeraddr, LINE_SIZE);
  /* */
  snprintf (peername, LINE_SIZE, "%s:%d", peeraddr, ntohs (addr.sin_port));

  printf ("%s connection open (handled by thread %d, pid is %d)\n",
	  peername, (int) pthread_self (), getpid ());

  for (;;)
    {
      ret = read (s, line, LINE_SIZE);
      if (ret == -1)
	{
	  perror ("read");
	  return 0;
	}
      else if (ret == 0)
	break;
      if (line[ret - 1] != '\n')
	line[ret++] = '\n';
      printf ("%s says: %.*s", peername, (int) ret, line);
    }

  printf ("%s connection closed\n", peername);
  close (s);
  return 0;
}

int
main ()
{
  int ear, ret;
  size_t len;
  struct sockaddr_in addr;

  ret = socket (AF_INET, SOCK_STREAM, 0);
  if (ret == -1)
    {
      perror ("socket");
      exit (1);
    }
  ear = ret;

  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = 0;
  ret = bind (ear, (struct sockaddr *) &addr, sizeof (addr));
  if (ret == -1)
    {
      perror ("bind");
      exit (1);
    }

  len = sizeof (addr);
  ret = getsockname (ear, (struct sockaddr *) &addr, &len);
  if (ret == -1)
    {
      perror ("getsockname");
      exit (1);
    }

  ret = listen (ear, 5);
  if (ret == -1)
    {
      perror ("listen");
      exit (1);
    }

  printf ("listening at port %d\n", ntohs (addr.sin_port));

  for (;;)
    {
      int *con;
      pthread_t t;

      ret = accept (ear, 0, 0);
      if (ret == -1)
	{
	  perror ("accept");
	  exit (1);
	}
      con = malloc (sizeof (int));
      if (!con)
	{
	  perror ("malloc");
	  exit (1);
	}
      *con = ret;

      ret = pthread_create (&t, 0, handle_connection, con);
      if (ret == -1)
	{
	  perror ("pthread_create");
	  exit (1);
	}
      ret = pthread_detach (t);
      if (ret == -1)
	{
	  perror ("pthread_detach");
	  exit (1);
	}
    }
}
