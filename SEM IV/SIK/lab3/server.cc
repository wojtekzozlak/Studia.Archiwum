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
#include <string.h>
#include <string>
#include <map>
#include <set>
#include <iostream>
#include "structures.h"
using namespace std;

#define TRUE 1
#define FALSE 0
#define BUF_SIZE 1024
#define MAX_CLIENTS (_POSIX_OPEN_MAX - 1)
#define TIMOUT 5


uint32_t dump;

static int finish = FALSE;
struct event_base *base;
struct timeval timeout = {TIMOUT, 0};
set<Client*> clients;
map<string, int> ids;

inline void
fatal (const char* str)
  {
    cout << "Fatal error!" << endl;
    perror (str);
    exit (EXIT_FAILURE);
  }

static void
catch_int (int sig)
  {
    cerr << "Signal " << sig << " catched. Termination started." << endl;
    if (event_base_loopbreak (base) == -1)
      fatal("event_base_loopbreak");
    return ;
  }

void
destroy_client (Client &str)
  {
    list<Handler*>::iterator it;
    for(it = str.recipe.begin (); it != str.recipe.end (); it++)
      delete *it;
    if (event_del (str.ev) == -1)
      fatal("event_del");
    if (str.c_msg != NULL) delete str.c_msg;
    if (str.c_id != NULL) delete str.c_id;
    close (str.fd);
    event_free (str.ev);
    clients.erase (&str);
    return ;
  }

void
shift_queue (int nr)
  {
    SendPlace sender;
    set<Client*>::iterator it;

    for(it = clients.begin (); it != clients.end (); it++)
      if((*it)->nr > nr)
        {
          (*it)->nr--;
          sender.handle (**it);
        }
    return ;
  }

void
cb_handler (evutil_socket_t fd, short what, void *arg)
  {
    int result;
    Client *str = (Client*) arg;

    if (what&EV_TIMEOUT && str->nr == 0)
      {
        cerr << "Client timout. Killing in process." << endl;
        shift_queue (str->nr);
        destroy_client (*str);
        return ;
      }

    do {
        result = (*(str->cur_handler))->handle (*str);
        if (result == OK)
          str->cur_handler++;
        else if (result == ERR || str->cur_handler == str->recipe.end ())
          {
            if (result == ERR)
              perror ("Error occured, client will be destroyed");
            destroy_client (*str);
            break;
          }
      } while (str->cur_handler != str->recipe.end () &&
                                    (*(str->cur_handler))->execution == NOW);
    if (str->cur_handler == str->recipe.end ())
      destroy_client (*str);

    return ;
  }

void
cb_listener (evutil_socket_t fd, short what, void *arg)
  {
    short msgsock;
    Client *c_arg;
    struct event *ev;

    if (what&EV_READ && !finish)
      {
        cerr << "Got an incoming connection. Creating a handling event." << endl;
        msgsock = accept (fd, (struct sockaddr *) NULL, (socklen_t *) NULL);
        if (-1 == msgsock)
          perror("accept");
        if (clients.size() == MAX_CLIENTS)
          {
            cerr << "Too many requests" << endl;
		        if (close (msgsock) < 0)
		          perror ("close");
          }
        else
          {
            c_arg = new Client ();
            c_arg->recipe.push_back (new Reader16((char *) &c_arg->msg_code));
            c_arg->recipe.push_back (new CodeChecker(0));
            c_arg->recipe.push_back (new SendPlace());
            c_arg->recipe.push_back (new QueueWait());
            c_arg->recipe.push_back (new Reader16((char *) &c_arg->msg_code));
            c_arg->recipe.push_back (new CodeChecker(2));
            c_arg->recipe.push_back (new Reader32((char *) &c_arg->id));
            c_arg->recipe.push_back (new Allocator(&c_arg->c_id, &c_arg->id));
            c_arg->recipe.push_back (new Reader(&c_arg->id, &c_arg->c_id));
            c_arg->recipe.push_back (new Reader32((char *) &c_arg->msg));
            c_arg->recipe.push_back (new Allocator(&c_arg->c_msg, &c_arg->msg));
            c_arg->recipe.push_back (new Reader(&c_arg->msg, &c_arg->c_msg));
            c_arg->recipe.push_back (new SendConfirmation());
            c_arg->recipe.push_back (new Finalizer());

            c_arg->nr = clients.size () + 1;
            c_arg->cur_handler = c_arg->recipe.begin ();
            c_arg->ids = &ids;
            c_arg->clients = &clients;

            c_arg->fd = msgsock;
            ev = event_new (base, msgsock, EV_TIMEOUT|EV_READ|EV_PERSIST,
                                                            cb_handler, c_arg);
            if (NULL == ev)
              fatal ("event_new");

            clients.insert (c_arg);

            c_arg->ev = ev;
            if (event_add (ev, &timeout) == -1)
              fatal ("event_add");
          }
      }
  }


int
main ()
{
  struct sockaddr_in server;
  size_t length;
  struct event *ev;
  short listen_socket;
  map<string, int>::iterator it;
  set<Client*>::iterator it2;

  /* Server ends on Ctrl-C */
  if (signal (SIGINT, catch_int) == SIG_ERR)
    fatal ("Unable to change signal handler");

  /* Create socket */
  listen_socket = socket (PF_INET, SOCK_STREAM, 0);
  if (listen_socket < 0)
    fatal ("Opening stream socket");

  /* Name socket using wildcards */
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htonl (INADDR_ANY);
  server.sin_port = 0;
  if (bind (listen_socket, (struct sockaddr *) &server,
                       (socklen_t) sizeof (server)) < 0)
    fatal ("Binding stream socket");

  /* Find out assigned port number and print it out */
  length = sizeof server;
  if (getsockname
      (listen_socket, (struct sockaddr *) &server, (socklen_t *) & length) < 0)
    fatal("Getting socket name");
  cerr << "Socket port #" << (unsigned) ntohs (server.sin_port) << endl;


  /* Start accepting connections */
  if (listen (listen_socket, 5) == -1)
    fatal ("Starting to listen");

  /* Create events base */
  base = event_base_new();
  if (NULL == base)
    fatal ("event_base_new");

  /* Create listening event */
  ev = event_new(base, listen_socket, EV_TIMEOUT|EV_READ|EV_PERSIST,
                                                            cb_listener, NULL);
  if (NULL == ev)
    fatal ("event_new");
  event_add(ev, NULL);
  event_base_loop(base, 0);

  cout << "Final report" << endl;
  for (it = ids.begin (); it != ids.end (); it++)
    cout << it->first << " : " <<  it->second << endl;

  while ( (it2 = clients.begin ()) != clients.end ())
    destroy_client (**it2);

  if (listen_socket >= 0)
    if (close (listen_socket) < 0)
      perror ("Closing main socket");
  exit (EXIT_SUCCESS);
}
