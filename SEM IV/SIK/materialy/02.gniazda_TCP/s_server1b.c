/*
 This program creates a socket and then begins an infinite loop. Each time
 through the loop it accepts a connection and prints out messages from it.
 When the connection breaks, or a termination message comes through, the
 program accepts a new connection.
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE   1024
#define QUEUE_LENGTH     5

int
main ()
{
  int     sock, msgsock;
  size_t  length;
  ssize_t rval;
  struct  sockaddr_in server;
  char    buf[BUFFER_SIZE];

  /* Create socket. */
  sock = socket (PF_INET, SOCK_STREAM, 0);
  if (sock < 0)
    {
      perror ("opening stream socket");
      exit (EXIT_FAILURE);
    }

  /* Name socket using wildcards.
     It is an Internet address. */
  server.sin_family = AF_INET;
  /* One computer can have several network addresses.
     Let all addresses be useable for this socket. */
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  /* Choose any of valid port numbers. */
  server.sin_port = 0;
  /* Associate the address with the socket. */
  if (bind (sock, (struct sockaddr *) &server, sizeof server) < 0)
    {
      perror ("binding stream socket");
      exit (EXIT_FAILURE);
    }

  /* Find out assigned port number and print it out. */
  length = sizeof server;
  if (getsockname (sock, (struct sockaddr *) &server, &length) < 0)
    {
      perror ("getting socket name");
      exit (EXIT_FAILURE);
    }
  /* Functions ntohx() transform data from network to host byte order. */
  printf ("Socket port #%u\n", (unsigned int) ntohs (server.sin_port));

  /* Start accepting connections. */
  if (listen (sock, QUEUE_LENGTH) < 0)
    {
      perror ("listening stream socket");
      exit (EXIT_FAILURE);
    }
  for (;;)
    {
      msgsock = accept (sock, NULL, NULL);
      if (msgsock < 0)
	perror ("accept");
      else
	do
	  {
	    memset (buf, 0, sizeof buf);
	    rval = read (msgsock, buf, sizeof buf);
            if (rval < 0)
	      perror ("reading stream message");
            else if (rval == 0)
	      printf ("Ending connection\n");
	    else
	      printf ("-->%.*s\n", (int)rval, buf);
	  }
	while (rval > 0);
      if (close (msgsock) < 0)
        perror ("closing client socket");
    }
  /* Since this program has an infinite loop, the socket "sock" is
     never explicitly closed. However, all sockets will be closed
     automatically when a process is killed or terminates normally. */
}
