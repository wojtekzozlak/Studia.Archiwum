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
#define FATHER           0
#define CHILD            1

static const char* process_name[] = {"IPv4", "IPv6"};

int main ()
{
  int                 sock, msgsock, domain, process_type;
  unsigned int        port;
  socklen_t           server_len, length;
  ssize_t             rval;
  char                buf[BUFFER_SIZE];
  pid_t               pid;
  struct sockaddr_in6 server_str6;
  struct sockaddr_in  server_str4;
  struct sockaddr*    server;


  // dwa procesy dla dwóch interfejsów
  switch(pid = fork())
    {
      case -1:
          perror ("fork");
          exit (EXIT_FAILURE);
          break;

      case 0:
          printf ("child process - IPv6\n");
          
          server_str6.sin6_family = AF_INET6;    /* rodzina           */
          server_str6.sin6_addr = in6addr_any;   /* adres do nasłuchu */
          server_str6.sin6_port = 0;             /* dowolny port      */
          server_len = sizeof server_str6;
          server = (struct sockaddr*) &server_str6;
          process_type = CHILD;
          domain = AF_INET6;

          break;

      default:
          printf ("father process - IPv4\n");

          server_str4.sin_family = AF_INET;                /* rodzina           */
          server_str4.sin_addr.s_addr = htonl(INADDR_ANY); /* adres do nasłuchu */
          server_str4.sin_port = 0;                        /* dowolny port      */
          server_len = sizeof server_str4;
          server = (struct sockaddr*) &server_str4;
          process_type = FATHER;
          domain = AF_INET;

          break;
    }

  /* Create socket. */
  sock = socket (domain, SOCK_STREAM, 0);
  if (sock < 0)
    {
      perror ("opening stream socket");
      exit (EXIT_FAILURE);
    }

  
  /* bindujemy adres do gniazda */
  if (bind (sock, server, server_len) < 0)
    {
      perror ("binding stream socket");
      exit (EXIT_FAILURE);
    }


  /* zbieramy numer portu */
  length = server_len;
  if (getsockname (sock, server, &length) < 0)
    {
      perror ("getting socket name");
      exit (EXIT_FAILURE);
    }
  if(process_type == CHILD)
    port = (unsigned int) ntohs (server_str6.sin6_port);
  else
    port = (unsigned int) ntohs (server_str4.sin_port);
  printf ("%s: socket port #%u\n", process_name[process_type], port);


  /* lecimy z akceptacją połączeń */
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
        printf("%s: connection accepted\n", process_name[process_type]);
	      do
	        {
	          memset (buf, 0, sizeof buf);
	          rval = read (msgsock, buf, sizeof buf);
                  if (rval < 0)
	            perror ("reading stream message");
                  else if (rval == 0)
	            printf ("Ending connection\n");
	          else
	            printf ("%s --> %.*s\n", process_name[process_type],
	                      (int)rval, buf);
	        }
      	while (rval > 0);
      if (close (msgsock) < 0)
        perror ("closing client socket");
    }

}
