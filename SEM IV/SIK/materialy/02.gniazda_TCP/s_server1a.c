#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>

#define TRUE 1

/*
 This program creates a socket and then begins an infinite loop. Each time
 through the loop it accepts a connection and prints out messages from it.
 When the connection breaks, or a termination message comes through, the
 program accepts a new connection.
*/

int
main ()
{
  int sock, length;
  struct sockaddr_in server;
  int msgsock;
  char buf[1024];
  int rval;

/* Create socket */
  sock = socket (PF_INET, SOCK_STREAM, 0);
  if (sock < 0)
    {
      perror ("opening stream socket");
      exit (1);
    }

/* Name socket using wildcards */
  /* it is an Internet address */
  server.sin_family = AF_INET;
  /* one computer can have several network addresses, let all addresses be
     useable for this socket */
  server.sin_addr.s_addr = INADDR_ANY;
  /* choose any valid port number */
  server.sin_port = 0;
  /* associate the address with the socket */
  if (bind (sock, (struct sockaddr *) &server, sizeof server) < 0)
    {
      perror ("binding stream socket");
      exit (1);
    }

/*Find out assigned port number and print it out*/
  length = sizeof server;
  if (getsockname (sock, (struct sockaddr *) &server, &length) < 0)
    {
      perror ("getting socket name");
      exit (1);
    }
/* functions ntohx() transform data from network to host byte order */
  printf ("Socket port #%d\n", ntohs (server.sin_port));

/*Start accepting connections */
  listen (sock, 5);
  do
    {
      msgsock = accept (sock, (struct sockaddr *) 0, (int *) 0);
      if (msgsock == -1)
	perror ("accept");
      else
	do
	  {
	    memset (buf, 0, sizeof buf);
	    if ((rval = read (msgsock, buf, 1024)) < 0)
	      perror ("reading stream message");
	    if (rval == 0)
	      printf ("Ending connection\n");
	    else
	      printf ("-->%s\n", buf);
	  }
	while (rval != 0);
      close (msgsock);
    }
  while (TRUE);
/* Since this program has an infinite loop, the socket "sock" is
   never explicitly closed. However, all sockets will be closed
   automatically when a process is killed or terminates normally.
*/
  exit (0);
}
