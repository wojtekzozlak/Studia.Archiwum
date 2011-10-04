/* Remote date server using UDP socket implementation.
   Usage: rserv port
*/

#include <sys/types.h>
#include <sys/fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <signal.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BSIZE        1024	/* Buffer size, oversized */

int
main (int argc, char *argv[])
{
  int sock;			/* Socket descriptor  */
  struct sockaddr_in server;	/* Server's address   */
  struct sockaddr_in client;	/* Client's address   */
  in_port_t server_port;	/* Port to connect to */
  char buffer[BSIZE];
  socklen_t client_len;
  ssize_t n;
  time_t time_buffer;

  if (argc != 2)
    {
      fprintf (stderr, "usage: %s port\n", argv[0]);
      exit (EXIT_FAILURE);
    }

  server_port = (in_port_t) atoi (argv[1]);

  /* Create a datagram socket */
  sock = socket (PF_INET, SOCK_DGRAM, 0);
  if (sock < 0)
    {
      perror ("socket");
      exit (EXIT_FAILURE);
    }

  /* Bind a local address */
  memset (&server, 0, sizeof server);
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htonl (INADDR_ANY);
  server.sin_port = htons (server_port);
  if (bind (sock, (struct sockaddr *) &server, sizeof server) < 0)
    {
      fprintf (stderr, "rdate server: bind failed\n");
      exit (EXIT_FAILURE);
    }

  for (;;)
    {
      /* Get request */
      memset (buffer, 0, sizeof (buffer));

      client_len = sizeof (client);
      n = recvfrom (sock, buffer, BSIZE, 0,
		    (struct sockaddr *) &client, &client_len);
      if (n <= 0)
	{
	  perror ("recvfrom");
	  exit (EXIT_FAILURE);
	}

      if (strcmp (buffer, "Date?") != 0)
	{
	  printf ("Bad request: %s\n", buffer);
	  exit (EXIT_FAILURE);
	}

      /* Send response */
      strcpy (buffer, "Time is ");
      time (&time_buffer);
      strcat (buffer, ctime (&time_buffer));

      if (sendto (sock, buffer, strlen (buffer) + 1, 0,
		  (struct sockaddr *) &client, sizeof client) < 0)
	{
	  perror ("sendto");
	  exit (EXIT_FAILURE);
	}

    }

  exit (EXIT_SUCCESS);
}
