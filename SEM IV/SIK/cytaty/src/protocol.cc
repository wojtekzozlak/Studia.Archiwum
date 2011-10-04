#include <arpa/inet.h>
#include "protocol.h"

void req_hton(REQ_MSG_CHUNK *msg)
{ msg->q_id = htonl(msg->q_id); }

void req_ntoh(REQ_MSG_CHUNK *msg)
{ msg->q_id = ntohl(msg->q_id); }

void resp_hton(RESP_MSG *msg)
{ msg->q_id = htonl(msg->q_id); }

void resp_ntoh(RESP_MSG *msg)
{ msg->q_id = ntohl(msg->q_id); }
