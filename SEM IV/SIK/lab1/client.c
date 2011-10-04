#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define BUFFER_SIZE      1024
#define HOST_NAME_LENGTH  256

static const char bye_string[] = "BYE";

int main (int argc, char *argv[])
{
  int sock, retcode, domain;
  struct addrinfo addr_hint, *addr;
  char line[BUFFER_SIZE], hostn[HOST_NAME_LENGTH];


  /* czy dość argumentów? */
  if (argc < 2)
    {
      printf ("Usage: %s port\n", argv[0]);
      return 0;
    }

  /* zbieramy nazwę hosta */
  if (gethostname (hostn, sizeof hostn) != 0)
    {
      perror ("gethostname");
      exit (EXIT_FAILURE);
    }
  printf ("host: %s\n\n", hostn);

  /* ok... bierzemy listę możliwych adresów */
  addr_hint.ai_flags = 0;
  addr_hint.ai_family = PF_UNSPEC;
  addr_hint.ai_socktype = SOCK_STREAM;
  addr_hint.ai_protocol = IPPROTO_TCP;
  addr_hint.ai_addrlen = 0;
  addr_hint.ai_addr = NULL;
  addr_hint.ai_canonname = NULL;
  addr_hint.ai_next = NULL;
  if ((retcode = getaddrinfo (hostn, NULL, &addr_hint, &addr)) != 0)
    {
      printf("%s\n", gai_strerror(retcode));
      exit (EXIT_FAILURE);
    }

  /* szukamy punktu zaczepienia */
  while (addr != NULL)
    {
      while (addr->ai_family != PF_INET)
        {
          addr = addr->ai_next;
          continue;
        }

      domain = addr->ai_family == PF_INET ? AF_INET : AF_INET6;
      printf("trying address...\n");

      /* tworzymy socket */
      sock = socket (domain, SOCK_STREAM, 0);
      if (sock < 0)
        {
          perror("socket failed");
          exit (EXIT_FAILURE);
        }
      ((struct sockaddr_in *) addr->ai_addr)->sin_port = htons (atoi (argv[1]));

      /* jeśli się nie udało, to próbujemy kolejny */
      if (connect (sock, addr->ai_addr, addr->ai_addrlen) != 0)
        {
          perror("connection");
          printf("\n");
          addr = addr->ai_next;
        }
      /* jeśl się udało, to koniec poszukiwań */
      else
        break;
    }
  if(addr == NULL)
    {
      printf ("fatal error: can't find compatible protocol\n");
      exit (EXIT_FAILURE);
    }


  do
    {
      printf ("line: ");
      fgets (line, sizeof line, stdin);
      if (write (sock, line, strlen (line)) < 0)
      	perror ("writing on stream socket");
    }
  while (strncmp (line, bye_string, sizeof bye_string - 1));

  if (close (sock) < 0)
    perror ("closing stream socket");

  return 0;
}
