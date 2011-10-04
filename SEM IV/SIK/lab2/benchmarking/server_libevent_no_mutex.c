#include <limits.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <event2/event.h>
#include <pthread.h>

#define TRUE 1
#define FALSE 0
#define BUF_SIZE 1024
#define MAX_CLIENTS 15000
long long readed = 0;
int finished = 0;

struct client
  {
    short fd;
    int nr;
    struct event *ev;
  };

static int finish = FALSE;
struct event_base *base;
int active_clients;
struct client *clients[MAX_CLIENTS];
short next_client;

void check(int i, char* msg)
  {
    if (0 != i)
      {
        perror (msg);
        exit (EXIT_FAILURE);
      }
    return ;
  }

static void
catch_int (int sig)
{
//  fprintf (stderr, "Signal %d catched. Termination started.\n", sig);
  check(event_base_loopbreak(base), "event loop break");
}

void
remove_client(struct client *arg)
  {
    if (next_client > 0)
      {
        clients[arg->nr] = clients[next_client - 1];
        clients[arg->nr]->nr = arg->nr;
      }
    clients[next_client - 1] = NULL;
    --next_client;
    free(arg);
  }

void
cb_handler(evutil_socket_t fd, short what, void *arg)
  {
    ssize_t rval;
    char buf[BUF_SIZE];
    struct client *c_arg = (struct client*) arg;
//    printf("handling\n");
//    fflush(stdout);
    
    rval = read (fd, buf, BUF_SIZE);
		if (rval < 0)
		  {
		    perror ("Reading stream message");
		    if (close (fd) < 0)
		      {
		        perror ("close");
    		    remove_client (c_arg);
		      }
		  }
		else if (rval == 0)
		  {
//		    fprintf (stderr, "Ending connection\n");
		    if (close (fd) < 0)
    			perror ("close");
    	  if (0 != event_del(c_arg->ev))
    	    {
    	      perror ("event del");
    	      exit (EXIT_FAILURE);
    	    }
    	  ++finished;
    	  if (finished == WAITING_FOR)
            check(event_base_loopbreak(base), "event loop break");
    	  
		    remove_client (c_arg);
		  }
    else
      {
        readed += (long long) rval;
//        printf ("-->%.*s \n", (int) rval, buf);
      }

//    fflush(stdout);
  }

void
cb_listener (evutil_socket_t fd, short what, void *arg)
  {
    short msgsock;
    struct client *c_arg;
    struct event *ev;

    if (what&EV_READ && !finish)
      {
//        printf ("Got an incoming connection. Creating a handling event.\n");
        msgsock = accept (fd, (struct sockaddr *) NULL, (socklen_t *) NULL);
        if (-1 == msgsock)
          perror("accept");
        if (active_clients == MAX_CLIENTS)
          {
//            printf ("Too many requests");
		        if (close (msgsock) < 0)
		          perror ("close");
          }
        else
          {
            c_arg = malloc(sizeof(struct client) * 2);
            c_arg->fd = msgsock;
            c_arg->nr = next_client;
            clients[next_client] = c_arg;
            ++next_client;
            ++active_clients;

            ev = event_new (base, msgsock, EV_TIMEOUT|EV_READ|EV_PERSIST,
                                                              cb_handler, c_arg);
            if (NULL == ev)
              {
                perror ("event_new");
                exit (EXIT_FAILURE);
              }
            c_arg->ev = ev;
            event_add (ev, NULL);
//            printf("Created under %d\n", c_arg->nr);
          }
      }

//    fflush (stdout);
  }

int
main (int argc, char *argv[])
{
  struct sockaddr_in server;
  int i;
  struct event *ev;
  short listen_socket;

  /* Check if port number is given. */
  if (argc < 2)
    {
      printf ("Usage: %s port\n", argv[0]);
      return 0;
    }

  
  /* get structures ready */
  for(i = 0; i < MAX_CLIENTS; i++)
    clients[i] = NULL;
  active_clients = 0;
  next_client = 0;
  

  /* Server ends on Ctrl-C */
  if (signal (SIGINT, catch_int) == SIG_ERR)
    {
      perror ("Unable to change signal handler\n");
      exit (EXIT_FAILURE);
    }

  /* Create socket */
  listen_socket = socket (PF_INET, SOCK_STREAM, 0);
  if (listen_socket < 0)
    {
      perror ("Opening stream socket");
      exit (EXIT_FAILURE);
    }

  /* Name socket using wildcards */
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htonl (INADDR_ANY);
  server.sin_port = htons (atoi (argv[1]));
  if (bind
      (listen_socket, (struct sockaddr *) &server,
       (socklen_t) sizeof (server)) < 0)
    {
      perror ("Binding stream socket");
      exit (EXIT_FAILURE);
    }

  /* Start accepting connections */
  if (listen (listen_socket, 5) == -1)
    {
      perror ("Starting to listen");
      exit (EXIT_FAILURE);
    }

  /* Create events base */
  base = event_base_new();
  if (NULL == base)
    {
      perror("event_base_new");
      exit (EXIT_FAILURE);
    }

  /* Create listening event */
  ev = event_new(base, listen_socket, EV_TIMEOUT|EV_READ|EV_PERSIST, cb_listener, NULL);
  if (NULL == ev)
    {
      perror ("event new");
      exit (EXIT_FAILURE);
    }
  check (event_add(ev, NULL), "event add");
  check (event_base_loop(base, 0), "event base loop");

  for (i = 0; i < next_client; i++)
    close(clients[i]->fd); /* we're already terminating, so forget about retval check */

  if (listen_socket >= 0)
    if (close (listen_socket) < 0)
      perror ("Closing main socket");

//  FILE *f = fopen ("result", "w+");
//  fprintf (f, "%lld", readed);
//  fclose (f);
  exit (EXIT_SUCCESS);
}
