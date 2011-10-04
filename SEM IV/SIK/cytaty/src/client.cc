#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <string>
#include <set>
#include <vector>
#include <iostream>
#include <sstream>
#include <poll.h>
#include "protocol.h"
#include "client.h"
using namespace std;

#define REQUEST_QUOTE 0
#define WAIT_FOR_FIRST 1
#define WAIT_FOR_REST 2
#define REQUEST_CHUNKS 3
#define SUCCESS 4
#define ERROR 5

#define MAX_TRIES_FIRST 5
#define MAX_TRIES_REST 10
#define TIMEOUT 1000
#define SOCK_RDY 1
#define SOCK_TOUT 0

#define ERR_CHECK(x, y) \
  if(-1 == (x)){ \
    throw QuotationsClientException(string(strerror(errno)).append(", ").append(y)); \
   };

QuotationsClient::~QuotationsClient()
{ finalize(); }

int QuotationsClient::getTime()
{  
  struct timeval tv;
  ERR_CHECK(gettimeofday(&tv, NULL), "Gettimeofday failed");
  return tv.tv_sec * 1000 + tv.tv_usec;
}

bool QuotationsClient::waitForSock(const int max, int timeout_state)
/* Uses poll to perform a wait for a socket with a given timeout. */
/*                                                                */
/* The [max] and [timeout_state] are used to determine a server   */
/* state when timeout occurs.                                     */
{
  struct pollfd pool;
  int ret, remaining_time = timeout_base + TIMEOUT - getTime();
  pool.fd = sock;
  pool.events = POLLIN;
  pool.revents = 0;

  if(0 >= remaining_time)
    ret = SOCK_TOUT;
  else
  {
    ret = poll(&pool, 1, remaining_time);
    ERR_CHECK(ret, "poll");
  }

  if(SOCK_TOUT == ret)
  {
    log("WAR: Timeout occured.\n");
    if(max <= ++tries)
    {
      log("ERR: Max tries reached!\n");
      state = ERROR;
    }
    else
    {
      log("Trying again.\n");
      state = timeout_state;
    }
    return false;
  }
  return true;
}

void QuotationsClient::addChunk()
/* Storages quotation chunk for further usage.         *
 *                                                     *
 * Chunks with invalid id are ignored and they are not *
 * reseting a timeout counter.                         */
{
  if(resp_msg.q_chunk >= msg.size())
  {
    log("WAR: Invalid chunk, skipping.");
    return ;
  }
  timeout_base = getTime();
  msg[resp_msg.q_chunk] = string(resp_msg.q_con, resp_msg.q_len);
  chunks.erase(resp_msg.q_chunk);
  return ;
}

void QuotationsClient::prepare(const char *server_name, in_port_t port, int log)
{
  struct addrinfo addr_hint, *addr;
  int ret;
  logFd = log;
  finalize(); /* Close previous socket. */

  /* Create socket */
  sock = socket(PF_INET, SOCK_DGRAM, 0);
  ERR_CHECK(sock, "socket");

  /* Bind a local address */
  client.sin_family = AF_INET;
  client.sin_addr.s_addr = htonl(INADDR_ANY);
  client.sin_port = 0;
  ERR_CHECK(bind(sock, (struct sockaddr *) &client, sizeof client), "bind");

  /* Get the server's address */
  memset(&addr_hint, 0, sizeof(addr_hint));
  addr_hint.ai_family = PF_INET;
  addr_hint.ai_socktype = SOCK_DGRAM;
  addr_hint.ai_protocol = IPPROTO_UDP;
  ret = getaddrinfo(server_name, NULL, &addr_hint, &addr);
  if(0 != ret)
    throw QuotationsClientException("getaddrinfo");
  memcpy(&server, addr->ai_addr, sizeof(server));
  server.sin_port = htons(port);
  return ;
}

void QuotationsClient::finalize()
/* Cleans up class internals. */
{
  if(sock != -1)
    close(sock);
  sock = -1;
  return ;
}

void QuotationsClient::log(const char *msg)
/* Logs up a message. */
{
  int len = strlen(msg);
  if(-1 == write(logFd, msg, len))
    throw QuotationsClientException("Write failed");
  return ;
}

void QuotationsClient::handleRequestQuote()
/* Handles a REQUEST_QUOTE state.       */
/*                                      */
/* Sends a request message to a server. */
{
  int n;
  log("Sending REQ_MSG request.\n");
  n = sendto(sock, &req_msg, sizeof(req_msg), 0,
             (struct sockaddr *) &server, sizeof(server));
  ERR_CHECK(n, "REQUEST_QUOTE sendto");
  timeout_base = getTime();
  state = WAIT_FOR_FIRST;
  return ;
}

void QuotationsClient::handleWaitForFirst()
/* Handles a WAIT_FOR_REST client state.                     */
/*                                                           */
/* Method waits for a socket for reading and then receives   */
/* a quotation chunk. Received chunk determines a qoutation  */
/* id for a current getQutation process. If quotations size  */
/* is equal one, the next state will be set to a `SUCCESS`.  */
/* If not, client will wait for a other chunks in a          */
/* `WAIT_FOR_REST` state. Timeout triggers a `REQUEST_QUOTE` */
{
  bool sock_rdy;
  int n;
  uint8_t i;

  log("Waiting for a first message.\n");
  sock_rdy = waitForSock(MAX_TRIES_FIRST, REQUEST_QUOTE);
  if(!sock_rdy)
    return ;
  else
  {
    n = recvfrom(sock, &resp_msg, sizeof(resp_msg), 0, NULL, 0);
    ERR_CHECK(n, "WAIT_FOR_FIRST recvfrom");
    req_msg_chunk.q_id = resp_msg.q_id; // still in net order!
    resp_ntoh(&resp_msg);
    q_id = resp_msg.q_id;
    for(i = 1; i <= resp_msg.q_size; i++)
    {
      chunks.insert(i);
    }
    msg.resize(resp_msg.q_size + 1);
    addChunk();
    if(0 == chunks.size())
      state = SUCCESS;
    else
    {
      timeout_base = getTime();
      state = WAIT_FOR_REST;
      tries = 0;
    }
  }
  return ;
}

void QuotationsClient::handleWaitForRest()
/* Handles a WAIT_FOR_REST state.                               */
/*                                                              */
/* Waits until a socket is ready for reading, and then receives */
/* a quotation chunk and tries to add it into a chunks storage. */
/* Timeout triggers a REQUEST_CHUNKS state.                     */
{
  bool sock_rdy;
  int n;
  socklen_t server_len;

  log("Waiting for further messages.\n");
  sock_rdy = waitForSock(MAX_TRIES_REST, REQUEST_CHUNKS);
  if(!sock_rdy)
    return ;
  else
  {
    server_len = sizeof(server);
    n = recvfrom(sock, &resp_msg, sizeof(resp_msg), 0,
                 (struct sockaddr *) &server, &server_len);
    ERR_CHECK(n, "WAIT_FOR_REST recvfrom");
    resp_ntoh(&resp_msg);
    if(resp_msg.q_id != q_id)
      return ;
    addChunk();
    if(0 == chunks.size())
      state = SUCCESS;
    else
      state = WAIT_FOR_REST;
  }
  return;
}

void QuotationsClient::handleRequestChunks()
/* Handles a REQUST_CHUNKS client state.                     */
/*                                                           */
/* Iterates over a chunks set in order to find which chunks  */
/* are missing. For each chunk a request to a server is sent.*/
{
  int n;
  set<uint8_t>::iterator chunks_it;

  log("Requesting missing chunks.\n");
  for(chunks_it = chunks.begin(); chunks_it != chunks.end(); chunks_it++)
  {
    req_msg_chunk.q_chunk = *chunks_it;
    n = sendto(sock, &req_msg_chunk, sizeof(req_msg_chunk), 0,
               (struct sockaddr *) &server, sizeof(server));
    ERR_CHECK(n, "REQUEST_CHUNKS sendto");
  }
  timeout_base = getTime();
  state = WAIT_FOR_REST;
  return ;
}

string QuotationsClient::handleSuccess()
/* Logs out a success message and builds up return value from */
/* stored chunks.                                             */
{
  stringstream str_builder("");
  uint8_t i;

  log("Success!\n");
  for(i = 1; i < msg.size(); i++)
    str_builder << msg[i];
  return str_builder.str();
}

string QuotationsClient::handleError()
/* Throws a standard QuotationsClientException. */
{
  log("Error!\n");
  throw QuotationsClientException("Can not complete a request!");
}

string QuotationsClient::getQuotation()
{
  req_msg.type = RANDOM_MSG;
  req_msg_chunk.type = SPECIFIC_MSG;
  state = REQUEST_QUOTE;
  q_id = -1;

  for(;;)
  {
    switch(state)
    {
      case REQUEST_QUOTE:
        handleRequestQuote();
        break;

      case WAIT_FOR_FIRST:
         handleWaitForFirst();
         break;

      case WAIT_FOR_REST:
        handleWaitForRest();
        break;

      case REQUEST_CHUNKS:
        handleRequestChunks();
        break;

      case SUCCESS:
        return handleSuccess();
        break;

      case ERROR:
        return handleError();
        break;

      default:
        break;
    }
  }
}
