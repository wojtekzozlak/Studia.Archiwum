#ifndef _COMMON_H_

#define _COMMON_H_

#include <stdio.h>
#include "err.h"

#define ID_REPO 1
#define FIRST_HANDLER 2

#define MKEY_RECEPTION 1234560
#define MKEY_GRAVEDIGGER 1234561
#define MKEY_HANDLER_IN 1234562
#define MKEY_HANDLER_OUT 1234563

#define RECEPTION 1

#define MESSAGE_SIZE (sizeof(Message) - sizeof(long))

#define HANDLER_REQ 1
#define HANDLER_OFFER 2
#define WANT_RESOURCES 3
#define RESOURCES 4
#define RESOURCES_AQUIRED 5
#define END_OF_WORK 6
#define RESIGNATION 7
#define SHUT_DOWN 8
#define JOIN_ME 9
#define CANT_SATISFY 10

struct MessageStr
/* only longs for array-message compatibility */
{
  long mtype; /* message type  */
  long comm;  /* command type  */
  long value; /* message value */
};
typedef struct MessageStr Message;

void set_message(Message *msg, long mtype, long comm, long value);
/* Procedure sets [msg] message structure. *
 *                                         *
 * MUTABLE PARAMETERS: msg                 */

int compare_long(const void *pa, const void *pb);
/* Qsort comparator for a long int type. */

void fatal_msg_expected(const char *type, long comm);
/* Ends program execution with the message-expected error. */

#endif
