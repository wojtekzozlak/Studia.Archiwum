/* Remote time client using UDP socket implementation.
   Usage: rtime hostname port
*/

#include <sys/types.h>
#include <sys/fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define BSIZE        1024	/* Buffer size, oversized */


int
main (int argc, char *argv[])
{
  int sock;			/* Socket descriptor  */
  struct sockaddr_in6 server;	/* Server's address   */
  struct sockaddr_in6 client;	/* Client's address   */
  struct addrinfo addr_hint, *addr;
  char *server_name;
  in_port_t server_port;	/* Port to connect to */
  char buffer[BSIZE];
  ssize_t count;
  socklen_t server_len;
  size_t length;

  if (argc != 3)
    {
      fprintf (stderr, "usage: %s hostname port\n", argv[0]);
      exit (EXIT_FAILURE);
    }

  server_name = argv[1];
  server_port = (in_port_t) atoi (argv[2]);

  /* Create a datagram socket */
  sock = socket (PF_INET6, SOCK_DGRAM, 0);

  /* Bind a local address */
  client.sin6_family = AF_INET6;
  server.sin6_flowinfo = 0;
  client.sin6_addr = in6addr_any;
  client.sin6_port = 0;		/* 0 says choose any port */
  if (bind (sock, (struct sockaddr *) &client, sizeof client) < 0)
    {
      fprintf (stderr, "rdate: bind failed\n");
      exit (EXIT_FAILURE);
    }

  /* Get the server's address */
  addr_hint.ai_flags = 0;
  addr_hint.ai_family = PF_INET6;
  addr_hint.ai_socktype = SOCK_DGRAM;
  addr_hint.ai_protocol = IPPROTO_UDP;
  addr_hint.ai_addrlen = 0;
  addr_hint.ai_addr = NULL;
  addr_hint.ai_canonname = NULL;
  addr_hint.ai_next = NULL;
  if (getaddrinfo (server_name, NULL, &addr_hint, &addr) != 0)
    {
      perror ("getaddrinfo");
      exit (EXIT_FAILURE);
    }
  memcpy (&server, addr->ai_addr, sizeof server);
  server.sin6_port = htons (server_port);

  strcpy (buffer, "Date?");
  length = strlen (buffer) + 1;

  /* Send the REQUEST */
  count = sendto (sock, buffer, length, 0,
		  (struct sockaddr *) &server, sizeof server);

  /* get response */

  server_len = sizeof (server);
  count = recvfrom (sock, buffer, BSIZE, 0,
		    (struct sockaddr *) &server, &server_len);

  /* Reach here when got a valid datagram */
  printf ("Server returned: %s\n", buffer);

  exit (EXIT_SUCCESS);
}
