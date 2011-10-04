#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>

/* run this program giving one parameter - port number. Program will try
to connect to the given port (on the same machine). It asks for lines of
text and sends them. BYE ends session */

main (argc, argv)
     int argc;
     char *argv[];
{
  int sock;
  struct sockaddr_in server;
  struct hostent *hp, *gethostbyname ();
  char buf[1024], line[1024], hostn[20];

  sock = socket (PF_INET, SOCK_STREAM, 0);
  if (sock < 0)
    {
      perror ("opening stream socket");
      exit (1);
    }
  server.sin_family = AF_INET;
  /* what is the name of our machine ? */
  if (gethostname (hostn, sizeof hostn) != 0)
    {
      perror ("gethostname");
      exit (1);
    }
  printf ("host: %s\n", hostn);

  /* what is the Internet address (xxx.xxx.xxx.xxx) of this machine */
  hp = gethostbyname (hostn);
  if (hp == 0)
    {
      fprintf (stderr, "%s : unknown host\n", hostn);
      exit (2);
    }
  memcpy ((char *) &server.sin_addr, (char *) hp->h_addr, hp->h_length);
  /* functions htonx() transform from host to network byte order */
  server.sin_port = htons (atoi (argv[1]));

  if (connect (sock, (struct sockaddr *) &server, sizeof server) < 0)
    {
      perror ("connecting stream socket");
      exit (1);
    }
  do
    {
      printf ("linia:");
      gets (line);
      if (write (sock, line, sizeof line) < 0)
	perror ("writing on stream socket");
    }
  while (strcmp (line, "BYE"));
  close (sock);

  exit (0);
}
