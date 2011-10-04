/* --Quotations storage protocol server--                            */
/*                                                                   */
/* Program listens on stdin in order to receive a `uint16_t` number  */
/* which determines which port will be used to provide a service.    */
/* Any data on stdin triggers rebinding to a new port.               */

#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <stdint.h>
#include <string.h>
#include <string>
#include <netinet/in.h>
#include <poll.h>
#include <signal.h>
#include <sys/prctl.h>
#include "database.h"
#include "protocol.h"

#define PERR(x) { perror(x); exit(EXIT_FAILURE); }
#define PERR_CHECK(x, y) if(-1 == (x)){ perror(y); exit(EXIT_FAILURE); };

/* Structure with a request-related data. */
struct RequestData {
  REQ_MSG *msg;
  REQ_MSG_CHUNK *c_msg;
  RESP_MSG response;
  QuotationsDB db;
  pair<int, string> quotation;
  int range_start, range_end;
  bool success;

  RequestData(const char *db) : db(db) {};
};

REQ_MSG * receive_msg(int sock, struct sockaddr *client, socklen_t *len)
/* Function receives a REQ_MSG or a REQ_MSG_CHUNK message from a [client] */
/* using a [sock] socket. */
{
  REQ_MSG *msg;
  REQ_MSG_CHUNK *c_msg;
  char buffer[BUFFER_SIZE];
  ssize_t n;

  n = recvfrom(sock, buffer, BUFFER_SIZE, 0, client, len);
  if(0 >= n)
    PERR("recvfrom");

  if(RANDOM_MSG == (uint8_t) *buffer)
  {
    msg = (REQ_MSG *) malloc(sizeof(REQ_MSG));
    if(NULL == msg)
      PERR("malloc");
    memcpy(msg, buffer, 1);
  }
  else
  {
    c_msg = (REQ_MSG_CHUNK *) malloc(sizeof(REQ_MSG_CHUNK));
    if(NULL == c_msg)
      PERR("malloc");
    msg = (REQ_MSG *) c_msg;
    memcpy(c_msg, buffer, n);
    req_ntoh(c_msg);
  }
  return msg;
}

void rebind_sock(int *sock, struct sockaddr_in *server)
/* Function reads 2 bytes*/
{
  uint8_t success = 0;
  uint16_t port;
  ssize_t n;
  int reuse = 1;

  while(0 == success)
  {
    PERR_CHECK(n = read(0, &port, 2), "read"); // CHECK!
    if(2 != n)
      PERR("broken port number");

    if(-1 != *sock)
      PERR_CHECK(close(*sock), "close");
    *sock = socket(PF_INET, SOCK_DGRAM, 0);
    PERR_CHECK(*sock, "socket");

    setsockopt(*sock, SOL_SOCKET, SO_REUSEADDR, (void *)&reuse, sizeof(reuse));

    server->sin_port = htons(port);
    fprintf(stderr, "Binding into port %d\n", port);

    if(-1 == bind(*sock, (struct sockaddr *) server, sizeof(sockaddr_in)))
      perror("bind");
    else
      success = 1;

   PERR_CHECK(write(1, &success, 1), "write");
  }

  fprintf(stderr, "Binded!\n");
  return ;
}

static void clean_up(int sig)
/* Cleans up server before termination. */
{
  fprintf(stderr, "Termination triggered!\n");
  close(2);
  exit(EXIT_SUCCESS);
}

void parse_request(RequestData &data)
/* Parses a client's request in order to set response-related data. */
{
  data.success = true;
  try
  {
    if(RANDOM_MSG == data.msg->type)
    {
      fprintf(stderr, "Random message requested\n");
      data.quotation = data.db.draw_quotation();
      data.range_start = 0;
      data.range_end = max((int) (data.quotation.second.size() - 1), 0) / MAX_LENGTH;
      data.response.q_size = data.range_end + 1;
    }
    else
    {
      data.c_msg = (REQ_MSG_CHUNK *) data.msg;
      fprintf(stderr, "Specific message requested (id: %d)\n", data.c_msg->q_id);
      data.quotation = data.db.draw_quotation(data.c_msg->q_id);
      data.range_start = data.range_end = data.c_msg->q_chunk - 1;
      data.response.q_size =
              max((int) (data.quotation.second.size() - 1), 0) / MAX_LENGTH + 1;
      if(data.response.q_size < data.range_start)
      {
        fprintf(stderr, "WAR: Bad chunk (chunk: %d, id: %d).\n",
                data.c_msg->q_chunk, data.c_msg->q_id);
        data.success = false;
      }
    }
  }
  catch(NoSuchQuotationException &e)
  { fprintf(stderr, "WAR: Cannot find quotation!\n"); data.success = false; }
  return ;
}

void send_response(RequestData &data, int sock, struct sockaddr_in &client)
/* Sends requested quotation chunks to a [client] via [sock] socket. */
{
  uint8_t chunk;
  for(chunk = data.range_start; chunk <= data.range_end; chunk++)
  {
    data.response.q_id = data.quotation.first;
    data.response.q_chunk = chunk + 1;
    data.response.q_len = min(MAX_LENGTH, (int) data.quotation.second.size() -
                                          MAX_LENGTH * chunk);
    resp_hton(&data.response);
    memcpy(&data.response.q_con, data.quotation.second.c_str() + chunk * MAX_LENGTH,
           data.response.q_len);

    fprintf(stderr, "Sending chunk...\n");
    PERR_CHECK(sendto(sock, &data.response, data.response.q_len + 7, 0,
                          (struct sockaddr *) &client, sizeof(client)),
               "sendto");
  }
  return ;
}

int main(int argc, char *argv[])
{
  struct sockaddr_in server;
  struct sockaddr_in client;
  struct pollfd pool[2];
  socklen_t client_len = sizeof(client);
  RequestData req_data("quotations.db");
  int ret, i;

  /* Preparing a server structure. */
  memset(&server, 0, sizeof(server));
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htonl(INADDR_ANY);

  /* Preparing a pool with a stdin for poll. */
  pool[0].fd = 0;
  pool[0].events = POLLIN;
  pool[0].revents = 0;
  pool[1].fd = -1;
  pool[1].events = POLLIN;
  pool[1].revents = 0;

  /* Setting up a cleaning handler */
  prctl(PR_SET_PDEATHSIG, SIGINT);
  if(SIG_ERR == signal(SIGINT, clean_up))
  {
    perror("Unable to change signal handler\n");
    exit(EXIT_FAILURE);
  }

  /* Binding a socket to a given port. */
  rebind_sock(&pool[1].fd, &server);

  fprintf(stderr, "Server started\n");
  for(;;)
  {
    for(i = 0; i < 2; i++)
      pool[i].revents = 0;

    ret = poll(pool, 2, 5000);
    if(ret < 0)
      perror("poll");
    else if(ret > 0)
    {
      if(pool[0].revents & POLLIN)
      {
        /* Rebind handling */
        rebind_sock(&pool[1].fd, &server);
      }
      else
      {
        /* Client handling */
        memset(&client, 0, sizeof(client));
        req_data.msg = receive_msg(pool[1].fd, (struct sockaddr *) &client,
                                   &client_len);
        parse_request(req_data);
        if(!req_data.success)
        {
          fprintf(stderr, "WAR: Request skipped.\n");
          free(req_data.msg);
          continue;
        }
        send_response(req_data, pool[1].fd, client);
        free(req_data.msg);
      }
    }
  }

  return 0;
}
