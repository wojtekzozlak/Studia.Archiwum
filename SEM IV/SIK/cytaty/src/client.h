#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <set>
#include <vector>
#include <netinet/in.h>
#include <sys/socket.h>
#include "protocol.h"
using namespace std;


class QuotationsClientException : public exception
/* Standard QuotationsClient exception. */
{
  private:
    string msg;

  public:
    QuotationsClientException(string msg) : msg(msg) {}
    ~QuotationsClientException() throw() {};

    const char * what() const throw()
    { return msg.c_str(); }
};

class QuotationsClient
/* Quotations protocol client implementation. */
{
  private:
    int logFd;
    bool waitForSock(const int max, int timeout_state);
    void addChunk();
    void log(const char *msg);
    void finalize();
    int getTime();

    set<uint8_t> chunks;
    vector<string> msg;
    int sock, state, tries, timeout_base;
    uint32_t q_id;
    struct sockaddr_in server;
    struct sockaddr_in client;
    REQ_MSG req_msg;
    REQ_MSG_CHUNK req_msg_chunk;
    RESP_MSG resp_msg;

    void handleRequestQuote();
    void handleWaitForFirst();
    void handleWaitForRest();
    void handleRequestChunks();
    string handleSuccess();
    string handleError();

  public:
    QuotationsClient() : sock(-1) {};
    ~QuotationsClient();

    /* Prepares internals for quotation requests on a given server. */
    void prepare(const char *server_name, in_port_t port, int log);

    /* Returns a random quotation from a quotations server. */
    string getQuotation();
};

#endif
