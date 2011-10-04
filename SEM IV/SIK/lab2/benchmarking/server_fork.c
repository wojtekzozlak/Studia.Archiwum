#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

#define TRUE             1
#define QUEUE_LENGTH     5

/*
 This program creates a socket and then begins an infinite loop. Each time
 through the loop it accepts a connection and prints out messages from it.
 When the connection breaks, or a termination message comes through, the
 program accepts a new connection.
*/


int
main (int argc, char *argv[])
{
  pid_t pid, pid1;
  int sock;
  struct sockaddr_in server;
  int msgsock;
  char buf[1024];
  ssize_t rval;
  int started = 0, i;
  int *x = malloc(sizeof(int) * 10000000);
  x[9999999] = 7;

  /* Check if port number is given. */
  if (argc < 2)
    {
      printf ("Usage: %s port\n", argv[0]);
      return 0;
    }


  /* we print a process identifier here */
//  printf ("My PID = %d\n", (int) getpid ());

  /* Create socket */
  sock = socket (PF_INET, SOCK_STREAM, 0);
  if (sock < 0)
    {
      perror ("opening stream socket");
      exit (EXIT_FAILURE);
    }

  /* Name socket using wildcards */
  /* it is an Internet address */
  server.sin_family = AF_INET;
  /* one computer can have several network addresses, let all addresses be
     useable for this socket */
  server.sin_addr.s_addr = INADDR_ANY;
  /* choose any valid port number */
  server.sin_port = htons (atoi(argv[1]));
  /* associate the address with the socket */
  if (bind (sock, (struct sockaddr *) &server, (int) sizeof server) < 0)
    {
      perror ("binding stream socket");
      exit (EXIT_FAILURE);
    }

  /* functions ntohx() transform data from network to host byte order */
//  printf ("Socket port #%d\n", (int) ntohs (server.sin_port));

  /*Start accepting connections */
  if (listen (sock, QUEUE_LENGTH) < 0)
    {
      perror ("setting listen queue length");
      exit (EXIT_FAILURE);
    };
  for (;;)
    {
      if (started == WAITING_FOR)
        {
          for (i = 0; i < WAITING_FOR; i++)
            wait (NULL);
          return 1;
        }
        
    
      msgsock = accept (sock, (struct sockaddr *) NULL, NULL);
      if (msgsock == -1)
	perror ("accept");
      else
	/* create a new process */
	switch (pid = fork ())
	  {
	  case -1:		/* blad */
      exit (-1);
	    break;

	  case 0:		/* child process */
	    close (sock);
	    pid1 = getpid ();
//	    printf ("I am child process. My PID = %d\n", (int) pid1);
//	    printf ("I am child process. The value returned by "
//		    "fork() = %d\n", (int) pid);

	    do
	      {
		memset (buf, 0, sizeof buf);
		rval = read (msgsock, buf, 1024);
		if (rval < 0)
		  perror ("reading stream message");
		else if (rval == 0);
//		  printf ("Ending connection\n");
		else;
//		  printf ("[%d]-->%.*s\n", (int) pid1, (int) rval, buf);
	      }
	    while (rval > 0);
	    close (msgsock);
	    return 0;

	  default:		/* parent process */
	    ++started;
//	    printf ("I am a parent process. The value returned by "
//		    "fork() = %d\n", (int) pid);
	    close (msgsock);

	    /* we do not wait for the child process to exit */
	  };			/*switch */
    }
  return 1;			/* just in case something goes wrong */
}
