#include <limits.h>
#include <poll.h>
#include <netinet/in.h>
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
#include <netinet/sctp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>

using namespace std;

#define BUF_SIZE 1024
#define MAX_CLIENTS (_POSIX_OPEN_MAX - 1)
#define TIMEOUT 1000


static bool        finish = false;
struct event_base  *base;
struct timeval     timeout = {TIMEOUT, 0};
Client*            first_client = NULL;
map<string, int>   ids;
uint32_t           next = 1;
uint16_t           listen_socket;

map< pair<uint16_t, uint16_t>, Client*>   clients;



static void
catch_int (int sig)
  {
    map<string, int>::iterator it;

    cerr << "Signal " << sig << " catched. Termination started." << endl;
    if (event_base_loopbreak (base) == -1)
      fatal("event_base_loopbreak");

    cout << "Final report" << endl;
    for (it = ids.begin (); it != ids.end (); it++)
      cout << it->first << " : " <<  it->second << endl;

    if (listen_socket >= 0)
      if (close (listen_socket) < 0)
        perror ("Closing main socket");
    exit (EXIT_SUCCESS);

    return ;
  }

void
send_place (short sd, uint16_t stream, uint32_t place)
  {
    char msg[6];

    uint16_t code = htons (1);
    place = htonl (place);
    *((uint16_t*) msg) = code;
    *((uint32_t*) (msg + 2)) = place;

    cerr << "Send place." << endl;
    if (-1 == sctp_sendmsg (sd, msg, 6, NULL, 0, 0, 0, stream, 0, 0))
      fatal ("sctp_sendmsg");
  }

void
send_confirmation (short sd, uint16_t stream, uint32_t nr)
  {
    char      msg[64];
    int       len;
    uint16_t  code = htons (3);

    cerr << "Sending confirmation." << endl;
    *((uint16_t *) msg) = code;
    len = sprintf (msg + 6, "Wiadomosc nr %d z tego id", nr);
    *((uint32_t *) (msg + 2)) = htonl (len);
    if (-1 == sctp_sendmsg (sd, (void *) msg, 6 + len, NULL, 0, 0, 0, stream, 0,
0))
      fatal ("sctp_sendmsg");
  }

bool
check_code(char *src, uint16_t code)
  {
    uint16_t scode = * ((uint16_t *) src);
    scode = ntohs (scode);
    return scode == code;
  }

void
shift_queue (uint32_t nr)
  {
    Client* str;
    map< pair<uint16_t, uint16_t>, Client*>::iterator it;
    for(it = clients.begin (); it != clients.end (); it++)
      {
        str = it->second;
        if(str->place > nr)
          {
            (str->place)--;
            if (1 == str->place)
              {
                first_client = str;
                str->state = WAITING_FOR_MSG;
              }
            send_place (str->socket, str->stream, str->place);
          }
      }
    return ;
  }

void
destroy_client (Client *str)
  {
    next--;
    shift_queue (str->place);
    clients.erase (make_pair (str->socket, str->stream));
    return ;
  }


void
erase_socket(uint16_t sock)
  {
    map< pair<uint16_t, uint16_t>, Client*>::iterator begin, end;
    int erased = 0;

    cout << "Erasing socket: " << sock << endl;

    begin = clients.begin ();
    while (begin != clients.end () && begin->first.first != sock)
      begin++;

    end = begin;
    
    while (end != clients.end () && end->first.first == sock)
      {
        shift_queue (end->second->place);
        next--;
        end++;
        erased++;
      }

    clients.erase(begin, end);
    cout << "Erased: " << erased << endl;
    return ;
  }

int
handle (Client *str, char* msg, int slen)
  {
    uint32_t id_s, com_s, len = slen;
    char *id, *com;
    int num;
    map<string, int>::iterator it;

    switch (str->state)
      {
        case INITIAL:
          if (len != 2 || !check_code (msg, 0))
            {
              cerr << "INITIAL" << endl;
              return ERR;
            }
          send_place (str->socket, str->stream, str->place);
          if (str->place == 1)
            {
              str->state = WAITING_FOR_MSG;
              first_client = str;
            }
          else
            str->state = WAITING_IN_QUEUE;
          break;

        case WAITING_IN_QUEUE:
          /* Rule violation */
          cerr << "WAITING!" << endl;
          return ERR;
          break;

        case WAITING_FOR_MSG:
          /* Parse message */
          if (len < 6 || !check_code (msg, 2))
            {
              cerr << "first" << endl;
              return ERR;
            }

          id_s = *((uint32_t *) (msg + 2));
          id_s = ntohl (id_s);
          id = new char(id_s + 1);

          if (len < 6 + id_s)
            {
              cerr << "second" << endl;
              return ERR;
            }
          memcpy (id, msg + 6, id_s);
          id[id_s] = '\0';

          if (len < 10 + id_s)
            {
              cerr << "third" << endl;
              return ERR;
            }
          com_s = *((uint32_t *) (msg + 6 + id_s));
          com_s = ntohl (com_s);
          com = new char(com_s + 1);

          if (len < 10 + id_s + com_s)
            {
              cerr << "forth" << endl;
              return ERR;
            }
          memcpy (com, msg + 10 + id_s, com_s);
          com[com_s] = '\0';

          cerr << "id: " << id << endl;
          cerr << "com: " << com << endl;

          /* Now send a message confirmation... */
          it = ids.find (string(id));
          if (it == ids.end())
            {
              ids.insert( pair<string, int> (string(id), 1) );
              num = 1;
            }
          else
            {
              num = ++(it->second);
            } 
          send_confirmation (str->socket, str->stream, num);
          destroy_client (str);
          break;

        default:
          break;
      };
    return OK;
  }

void
cb_handler (evutil_socket_t fd, short what, void *arg)
  {
    handler                 *str = (handler *) arg;
    char                    *msg = NULL;
    int                     msg_size = 0;
    uint16_t                stream;
    struct sctp_sndrcvinfo  info;
    map< pair<uint16_t, uint16_t>, Client*>::iterator it;
    
    if (what&EV_TIMEOUT && NULL != first_client &&
                                             first_client->socket == fd)
      {
        cerr << "Client timout. Killing in process." << endl;
        if (-1 == event_del (str->ev))
          fatal ("event_del");
        erase_socket (fd);
        close (fd);
        return ;
      }

    /* firstly we have to receive a whole message   */
    /* then we'll delegate it to the proper handler */
    cerr << "Handling transmission ";
    recv_msg (fd, &msg, &msg_size, &info);
    if (0 == msg_size)
      {
        cerr << "\nConnection lost.\n" << endl;
        if (-1 == event_del (str->ev))
          fatal ("event_del");
        erase_socket (fd);
        close (fd);
        return ;
      }

    /* Find a client */
    stream = info.sinfo_stream;
    cerr << "[socket " << fd << ", stream " << stream << "]" << endl;
    it = clients.find (make_pair (fd, stream));
    if (it == clients.end ())
      {
        clients.insert( make_pair (make_pair (fd, stream), new Client(fd, stream, next++) ));
        it = clients.find (make_pair (fd, stream));
        cerr << "Client created" << endl;
      }
    if (ERR == handle (it->second, msg, msg_size))
      {
        if (-1 == event_del (str->ev))
          fatal ("event_del");
        erase_socket (fd);
        close (fd);
      }

    cerr << "Handled!" << endl << endl;
    return ;
  }  

void
cb_listener (evutil_socket_t fd, short what, void *arg)
  {
    short msgsock;
    struct event *ev;
    handler *str;

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
            str = new handler;
            ev = event_new (base, msgsock, EV_TIMEOUT|EV_READ|EV_PERSIST,
                                                      cb_handler, str);
            str->ev = ev;
            if (NULL == ev)
              fatal ("event_new");

            if (-1 == event_add (ev, &timeout))
              fatal ("event_add");
          }
      }
  }


int
main ()
{
  struct sockaddr_in6 server;
  size_t length;
  struct event *ev;
  map<string, int>::iterator it;
  set<Client*>::iterator it2;
  struct sctp_event_subscribe events;

  /* Server ends on Ctrl-C */
  if (signal (SIGINT, catch_int) == SIG_ERR)
    fatal ("Unable to change signal handler");

  /* Create socket */
  listen_socket = socket (AF_INET6, SOCK_STREAM, IPPROTO_SCTP);
  if (listen_socket < 0)
    fatal ("Opening stream socket");


  /* Name socket using wildcards */
  server.sin6_family = AF_INET6;
  server.sin6_addr = in6addr_any;
  server.sin6_port = 0;
  if (bind (listen_socket, (struct sockaddr *) &server,
                       (socklen_t) sizeof (server)) < 0)
    fatal ("Binding stream socket");

  /* Enable SCTP event. */
  memset ((void *) &events, 0, sizeof (events));
  events.sctp_data_io_event = 1;
  setsockopt (listen_socket, SOL_SCTP, SCTP_EVENTS,
                  (const void *) &events, sizeof (events));

  /* Find out assigned port number and print it out */
  length = sizeof server;
  if (getsockname
      (listen_socket, (struct sockaddr *) &server, (socklen_t *) & length) < 0)
    fatal("Getting socket name");
  cerr << "Socket port #" << (unsigned) ntohs (server.sin6_port) << endl;


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

  return 0;
}
