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
#include <netinet/sctp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "structures.h"
#include <map>
using namespace std;

#define BUFFER_SIZE      1024
#define HOST_NAME_LENGTH  256

static const char bye_string[] = "BYE";

int
main (int argc, char *argv[])
{
  int sock;
  char *line;
  struct sockaddr_in servaddr;
  struct sctp_event_subscribe events;

  /* Check if port number is given. */
  if (argc < 2)
    {
      printf ("Usage: %s port\n", argv[0]);
      return 0;
    }

  sock = socket (AF_INET, SOCK_STREAM, IPPROTO_SCTP);
  if (sock < 0)
    {
      perror ("opening stream socket");
      exit (EXIT_FAILURE);
    }

  /* Enable SCTP event. */
  memset ((void *) &events, 0, sizeof (events));
  events.sctp_data_io_event = 1;
  setsockopt (sock, SOL_SCTP, SCTP_EVENTS,
                  (const void *) &events, sizeof (events));


  /* Specify the peer endpoint to which we'll connect */
  bzero( (void *)&servaddr, sizeof(servaddr) );
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons (atoi (argv[1]));
  servaddr.sin_addr.s_addr = inet_addr ("127.0.0.1");

  uint16_t code;
  uint32_t place;
  uint32_t id;
  uint32_t msg;
  size_t len;
  int ret, flags, next_stream = 0, stream;
  char *full_msg;
  struct sctp_sndrcvinfo info;
  map<string, int> streams;
  map<string, int>::iterator it;

  if (connect (sock, (struct sockaddr *)&servaddr, sizeof (servaddr)) != 0)
    {
      perror ("connecting stream socket");
      exit (EXIT_FAILURE);
    }


while (true)
{
  char *in_id = NULL, *in_msg = NULL;
  size_t buf_len;
  place = 0;

  len = getline (&in_id, &buf_len, stdin);
  in_id[len - 1] = '\0';
  len = getline (&in_msg, &buf_len, stdin);
  in_msg[len - 1] = '\0';

  it = streams.find (string (in_id));
  if (it == streams.end ())
    {
      streams.insert (make_pair (string(in_id), next_stream));
      stream = next_stream++;
    }
  else
    stream = it->second;

  if (in_id[0] == '\0')
    break;

  code = htons (0);
  if (-1 == sctp_sendmsg (sock, &code, 2, NULL, 0, 0, 0, stream, 0, 0))
    perror ("writing on stream socket");

  line = new char[512];
  while (place != 1)
    {
      printf("waiting for place...\n");
      ret = sctp_recvmsg (sock, line, 512, NULL, 0, &info, &flags);
      if (6 != ret)
        fatal("Unexpected message");

      place = ntohl(*((uint32_t*) (line + 2)));
      printf("current place: %d\n", place);
    }
  delete line;
  sleep(1);

  code = htons (2);
  id = htonl (strlen (in_id));
  msg = htonl (strlen (in_msg));
  len = 10 + strlen (in_id) + strlen (in_msg);
  full_msg = (char *) malloc (len);

  memcpy (full_msg, &code, 2);
  memcpy (full_msg + 2, &id, 4);
  memcpy (full_msg + 6, in_id, strlen (in_id));
  memcpy (full_msg + 6 + strlen (in_id), &msg, 4);
  memcpy (full_msg + 6 + strlen (in_id) + 4, in_msg, strlen (in_msg));

  sctp_sendmsg (sock, (void *) full_msg, len, NULL, 0, 0, 0, stream, 0, 0);
  sleep(1);
  recv_msg (sock, &line, &ret, &info);
  printf("%s\n\n\n", line + 6);

  fflush(stdout);

  delete line;
  free(in_id);
  free(in_msg);
}

  printf("closing\n");
  if (close (sock) < 0)
    perror ("closing stream socket");

  return 0;
}
