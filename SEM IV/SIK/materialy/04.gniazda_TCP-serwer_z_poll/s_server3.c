/*
 This program uses poll() to check that someone is trying to connect
 before calling accept.
*/

#include <poll.h>
#include <sys/socket.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUF_SIZE 1024
#define TRUE 1
#define FALSE 0

static int sock = -1;
static int run_server = TRUE;

/* Ctrl-C signal handler */
static void
catch_int (int sig)
{
  /* In signal handler use only async-signal-safe functions.
     Functions like exit, printf, fprintf are not safe. */
  static const char *const diagnostic = "Signal catched\n";
  write (1, diagnostic, strlen (diagnostic));
  run_server = FALSE;
}

int
main ()
{
  struct pollfd fd[1];
  struct sockaddr_in server;
  char buf[BUF_SIZE];
  int msgsock;
  size_t length;
  ssize_t rval;

  if (signal (SIGINT, catch_int) == SIG_ERR)
    {
      perror ("Unable to change signal handler");
      exit (EXIT_FAILURE);
    }

  /* Create socket */
  sock = socket (PF_INET, SOCK_STREAM, 0);
  if (sock < 0)
    {
      perror ("Opening stream socket");
      exit (EXIT_FAILURE);
    }

  /* Name socket using wildcards */
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htonl (INADDR_ANY);
  server.sin_port = 0;
  if (bind (sock, (struct sockaddr *) &server, (socklen_t) sizeof server) < 0)
    {
      perror ("Binding stream socket");
      exit (EXIT_FAILURE);
    }

  /* Find out assigned port number and print it out */
  length = sizeof server;
  if (getsockname (sock, (struct sockaddr *) &server, (socklen_t *) & length)
      < 0)
    {
      perror ("Getting socket name");
      exit (EXIT_FAILURE);
    }
  printf ("Socket port #%d\n", (int) ntohs (server.sin_port));

  /* Start accepting connections */
  if (listen (sock, 5) < 0)
    {
      perror ("Starting to listen");
      exit (EXIT_FAILURE);
    }

  while (run_server)
    {
      fd[0].fd = sock;
      fd[0].events = POLLIN;
      fd[0].revents = 0;
      /* If run_server becomes FALSE before entering poll and poll is called
         with negative (infinite) timeout, poll could be blocked forever.
         Wait max 5000 ms. */
      rval = poll (fd, sizeof (fd) / sizeof (fd[0]), 5000);
      if (rval < 0)
	{
	  perror ("poll");
	}
      else if (rval == 1 && (fd[0].revents & POLLIN))
	{
	  msgsock = accept (sock, (struct sockaddr *) 0, (socklen_t *) 0);
	  if (msgsock < 0)
	    perror ("accept");
	  else
	    do
	      if ((rval = read (msgsock, buf, BUF_SIZE)) < 0)
		perror ("Reading stream message");
	      else if (rval == 0)
		fprintf (stderr, "Ending connection\n");
	      else
		printf ("-->%.*s\n", (int) rval, buf);
	    while (rval > 0);
	  if (close (msgsock) < 0)
	    fprintf (stderr, "Error in closing client socket,"
		     " but server can still operate\n");
	}
      else
	fprintf (stderr, "Do something else\n");
    }

  fprintf (stderr, "Ending program\n");
  if (sock != -1)
    if (close (sock) < 0)
      fprintf (stderr, "Error in closing main socket\n");
  exit (EXIT_SUCCESS);
}
