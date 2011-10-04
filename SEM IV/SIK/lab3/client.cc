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
#include <iostream>
using namespace std;

#define BUFFER_SIZE      1024
#define HOST_NAME_LENGTH  256

static const char bye_string[] = "BYE";

int
main (int argc, char *argv[])
{
  int sock;
  struct addrinfo addr_hint, *addr;
  char line[BUFFER_SIZE], hostn[HOST_NAME_LENGTH];

  /* Check if port number is given. */
  if (argc < 4)
    {
      printf ("Usage: %s port id msg\n", argv[0]);
      return 0;
    }

  sock = socket (PF_INET, SOCK_STREAM, 0);
  if (sock < 0)
    {
      perror ("opening stream socket");
      exit (EXIT_FAILURE);
    }

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

  uint16_t code;
  uint32_t place = 0;
  uint32_t id;
  uint32_t msg;
  char *c_msg;

  if (connect (sock, addr->ai_addr, addr->ai_addrlen) != 0)
    {
      perror ("connecting stream socket");
      exit (EXIT_FAILURE);
    }

      code = htons (0);
      if (write (sock, &code, sizeof(unsigned short)) < 0)
	      perror ("writing on stream socket");

      while (place != 1)
        {
          printf("waiting for place...\n");
          if (read (sock, line, 6) <= 0)
            perror ("reading on stream socket");
          place = ntohl(*((uint32_t*) (line + 2)));
          printf("current place: %d\n", place);
        }
      sleep(1);

      code = htons (2);
      id = htonl (strlen (argv[2]));
      msg = htonl (strlen (argv[3]));

      write (sock, &code, 2);
      write (sock, &id, 4);
      write (sock, argv[2], strlen (argv[2]));
      write (sock, &msg, 4);
      write (sock, argv[3], strlen (argv[3]));

      read (sock, &code, 2);
      read (sock, &msg, 4);
      msg = ntohl (msg);
      c_msg = new char[msg + 1];
      read (sock, c_msg, msg);
      c_msg[msg] = '\0';
      cout << c_msg << endl;

  if (close (sock) < 0)
    perror ("closing stream socket");

  return 0;
}
