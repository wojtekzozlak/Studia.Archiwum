#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdlib.h>
#include <stdint.h>

#define BUFFER_SIZE 10
#define MAX_LENGTH (BUFFER_SIZE - 7)
#define RANDOM_MSG 0
#define SPECIFIC_MSG 1

#pragma pack(push, 1)
struct REQ_MSG_T
{
  uint8_t type;
};
typedef struct REQ_MSG_T REQ_MSG;

struct REQ_MSG_CHUNK_T
{
  uint8_t  type;
  uint32_t q_id;
  uint8_t  q_chunk;
};
typedef struct REQ_MSG_CHUNK_T REQ_MSG_CHUNK;

struct RESP_MSG_T
{
  uint32_t q_id;
  uint8_t  q_size;
  uint8_t  q_chunk;
  uint8_t  q_len;
  char     q_con[MAX_LENGTH];
};
typedef struct RESP_MSG_T RESP_MSG;
#pragma pack(pop)

/* Function which converts structures from host to net order and vice versa */
void req_hton(REQ_MSG_CHUNK *msg);
void req_ntoh(REQ_MSG_CHUNK *msg);
void resp_hton(RESP_MSG *msg);
void resp_ntoh(RESP_MSG *msg);

#endif
