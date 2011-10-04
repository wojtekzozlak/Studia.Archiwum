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

#define PORT 6666

static const char* process_name[] = {"IPv6", "IPv4"};

int main ()
{
  int                 sock, sock6, sock4, msgsock, process_type, on;
  unsigned int        port;
  socklen_t           length;
  ssize_t             rval;
  char                buf[BUFFER_SIZE];
  pid_t               pid;
  struct sockaddr_in6 server_str6;
  struct sockaddr_in  server_str4;

  /* SOCKET 6 */
  server_str6.sin6_family = AF_INET6;    /* rodzina           */
  server_str6.sin6_addr = in6addr_any;   /* adres do nasłuchu */
  server_str6.sin6_port = 0;             /* dowolny port      */

  sock6 = socket (AF_INET6, SOCK_STREAM, 0);
  if (sock6 < 0)
    {
      perror ("opening stream socket");
      exit (EXIT_FAILURE);
    }
  if (-1 == setsockopt(sock6, IPPROTO_IPV6, IPV6_V6ONLY, (char *)&on,  sizeof(on)))
    {
      perror ("setsockopt");
      exit (EXIT_FAILURE);
    }

  if (bind (sock6, (struct sockaddr*) &server_str6, sizeof(server_str6)) < 0)
    {
      perror ("binding stream socket");
      exit (EXIT_FAILURE);
    }

  /* zbieramy numer portu */
  length = sizeof(server_str6);
  if (getsockname (sock6, (struct sockaddr*) &server_str6, &length) < 0)
    {
      perror ("getting socket name");
      exit (EXIT_FAILURE);
    }

  port = (unsigned int) ntohs (server_str6.sin6_port);
  printf ("port #%u\n", port);


  /* SOCKET 4 */
  server_str4.sin_family = AF_INET;                /* rodzina           */
  server_str4.sin_addr.s_addr = htonl(INADDR_ANY); /* adres do nasłuchu */
  server_str4.sin_port = server_str6.sin6_port;    /* port taki sam     */


  sock4 = socket (AF_INET, SOCK_STREAM, 0);
  if (sock4 < 0)
    {
      perror ("opening stream socket");
      exit (EXIT_FAILURE);
    }

  if (bind (sock4, (struct sockaddr*) &server_str4, sizeof(server_str4)) < 0)
    {
      perror ("binding stream socket");
      exit (EXIT_FAILURE);
    }


  switch(pid = fork())
    {
      case -1:
        printf ("FATAL ERROR\n");
        exit (EXIT_FAILURE);
        break;

      case 0:
        process_type = CHILD;
        sock = sock4;
        break;

      default:
        process_type = FATHER;
        sock = sock6;
        break;
    }


  /*  lecimy z akceptacją połączeń */
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


  return 0;
}
