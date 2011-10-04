/*
 Run this program giving one parameter - port number. Program will try
 to connect to the given port (on the same machine). It asks for lines of
 text and sends them. BYE ends session.
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE      1024
#define HOST_NAME_LENGTH  256

static const char bye_string[] = "BYE";

int
main (int argc, char *argv[])
{
  int sock, i, repetitions, tries, j;
  struct addrinfo addr_hint, *addr;
  char hostn[HOST_NAME_LENGTH], *line = "I WANT SOME COOKIES";

  /* Check if port number is given. */
  if (argc < 4)
    {
      printf ("Usage: %s port repetitions tries\n", argv[0]);
      return 0;
    }

  repetitions = atoi (argv[2]);
  tries = atoi (argv[3]);
  

  /* What is the name of our machine? */
  if (gethostname (hostn, sizeof hostn) != 0)
    {
      perror ("gethostname");
      exit (EXIT_FAILURE);
    }
  printf ("host: %s\n", hostn);

  /* What is the Internet address of this machine? */
  addr_hint.ai_flags = 0;
  addr_hint.ai_family = PF_INET;
  addr_hint.ai_socktype = SOCK_STREAM;
  addr_hint.ai_protocol = IPPROTO_TCP;
  addr_hint.ai_addrlen = 0;
  addr_hint.ai_addr = NULL;
  addr_hint.ai_canonname = NULL;
  addr_hint.ai_next = NULL;
  if (getaddrinfo (hostn, NULL, &addr_hint, &addr) != 0)
    {
      perror ("getaddrinfo");
      exit (EXIT_FAILURE);
    }

  /* Functions htonx() transform from host to network byte order. */
  ((struct sockaddr_in *) addr->ai_addr)->sin_port = htons (atoi (argv[1]));

for(j = 0; j < tries; j++)
{
  sock = socket (PF_INET, SOCK_STREAM, 0);
  if (sock < 0)
    {
      perror ("opening stream socket");
      exit (EXIT_FAILURE);
    }

  if (connect (sock, addr->ai_addr, addr->ai_addrlen) != 0)
    {
      perror ("connecting stream socket");
      exit (EXIT_FAILURE);
    }

  for (i = 0; i < repetitions; i++)
    if (write (sock, line, strlen (line)) < 0)
  	  perror ("writing on stream socket");

  if (close (sock) < 0)
    perror ("closing stream socket");
}

  return 0;
}
