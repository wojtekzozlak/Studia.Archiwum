#include <limits.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0
#define BUF_SIZE 10240
#define MAX_CLIENTS 1000
long long readed = 0;
int finished = 0;

static int finish = FALSE;

/* Signal handler */
static void
catch_int (int sig)
{
  finish = TRUE;
  fprintf (stderr,
	   "Signal %d catched. No new connections will be accepted.\n", sig);
}

int
main (int argc, char *argv[])
{
  struct pollfd client[MAX_CLIENTS];
  struct sockaddr_in server;
  char buf[BUF_SIZE];
  size_t length;
  ssize_t rval;
  int msgsock, activeClients, i, ret;

  /* Check if port number is given. */
  if (argc < 2)
    {
      printf ("Usage: %s port\n", argv[0]);
      return 0;
    }

  /* Server ends on Ctrl-C */
  if (signal (SIGINT, catch_int) == SIG_ERR)
    {
      perror ("Unable to change signal handler\n");
      exit (EXIT_FAILURE);
    }

  /* Init client table, client[0] is main socket */
  for (i = 0; i < MAX_CLIENTS; ++i)
    {
      client[i].fd = -1;
      client[i].events = POLLIN;
      client[i].revents = 0;
    }
  activeClients = 0;

  /* Create socket */
  client[0].fd = socket (PF_INET, SOCK_STREAM, 0);
  if (client[0].fd < 0)
    {
      perror ("Opening stream socket");
      exit (EXIT_FAILURE);
    }

  /* Name socket using wildcards */
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htonl (INADDR_ANY);
  server.sin_port = htons (atoi (argv[1]));
  if (bind
      (client[0].fd, (struct sockaddr *) &server,
       (socklen_t) sizeof (server)) < 0)
    {
      perror ("Binding stream socket");
      exit (EXIT_FAILURE);
    }

  /* Find out assigned port number and print it out */
  length = sizeof server;
  if (getsockname
      (client[0].fd, (struct sockaddr *) &server, (socklen_t *) & length) < 0)
    {
      perror ("Getting socket name");
      exit (EXIT_FAILURE);
    }
  fprintf (stderr, "Socket port #%u\n", (unsigned) ntohs (server.sin_port));

  /* Start accepting connections */
  if (listen (client[0].fd, 5) == -1)
    {
      perror ("Starting to listen");
      exit (EXIT_FAILURE);
    }

  /* Serve */
  do
    {
      for (i = 0; i < MAX_CLIENTS; ++i)
	client[i].revents = 0;
      if (finish == TRUE && client[0].fd >= 0)
	{
	  if (close (client[0].fd) < 0)
	    perror ("close");
	  client[0].fd = -1;
	}

      /* Set timeout value 5000 ms */
      ret = poll (client, MAX_CLIENTS, 5000);
      if (ret < 0)
	perror ("poll");
      else if (ret > 0)
	{
	  if (finish == FALSE && (client[0].revents & POLLIN))
	    {
	      msgsock =
		accept (client[0].fd, (struct sockaddr *) 0, (socklen_t *) 0);
	      if (msgsock == -1)
		perror ("accept");
	      else
		{
		  for (i = 1; i < MAX_CLIENTS; ++i)
		    {
		      if (client[i].fd == -1)
			{
			  client[i].fd = msgsock;
			  ++activeClients;
			  break;
			}
		    }
		  if (i >= MAX_CLIENTS)
		    {
		      fprintf (stderr, "Too many clients\n");
		      if (close (msgsock) < 0)
			perror ("close");
		    }
		}
	    }
	  for (i = 1; i < MAX_CLIENTS; ++i)
	    {
	      if (client[i].fd != -1
		  && (client[i].revents & (POLLIN | POLLERR)))
		{
		  rval = read (client[i].fd, buf, BUF_SIZE);
		  if (rval < 0)
		    {
		      perror ("Reading stream message");
		      if (close (client[i].fd) < 0)
      			perror ("close");
		      client[i].fd = -1;
		      --activeClients;
		    }
		  else if (rval == 0)
		    {
		      fprintf (stderr, "Ending connection\n");
		      if (close (client[i].fd) < 0)
			perror ("close");
		      client[i].fd = -1;
		      --activeClients;
		      ++finished;
		      if (finished == WAITING_FOR)
		        finish = TRUE;
		    }
		  else
		    {
  		    printf ("-->%.*s\n", (int) rval, buf);
          readed += rval;
  		  }
		    
		}
	    }
	}
      else;
	fprintf (stderr, "Do something else\n");
    }
  while (finish == FALSE || activeClients > 0);

  if (client[0].fd >= 0)
    if (close (client[0].fd) < 0)
      perror ("Closing main socket");

  FILE *f = fopen ("result", "w+");
  fprintf (f, "%lld", readed);
  fclose (f);
  exit (EXIT_SUCCESS);
}
