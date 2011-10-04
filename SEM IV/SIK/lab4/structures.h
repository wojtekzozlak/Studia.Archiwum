#include <netinet/sctp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <event2/event.h>
using namespace std;

#define NOW  0
#define READ 1

#define MAX_TRIES 5


#define INITIAL 0
#define WAITING_IN_QUEUE 1
#define WAITING_FOR_MSG 2

#define ERR 0
#define OK 1

struct
handler
  {
    event *ev;
  };

struct
Client
  {
    int state;
    short socket;
    uint16_t stream;
    uint32_t place;
    event *ev;

    Client(short socket, uint16_t stream, uint32_t place) :
      state(INITIAL), socket(socket), stream(stream), place(place) {};
  };

inline void
fatal (const char* str)
  {
    cout << "Fatal error!" << endl;
    perror (str);
    exit (EXIT_FAILURE);
  }

void
swap_ptr (char *a, char *b)
  {
    char *c = a;
    a = b;
    b = c;
    return ;
  }

void
recv_msg(short socket, char **msg, int *msg_size, struct sctp_sndrcvinfo *info)
  {
    int flags, ret, size = 0, capacity = 1024;
    char *mmsg, *buff;

    mmsg = new char[capacity + 1];
    do
      {
        ret = sctp_recvmsg (socket, (void *) (mmsg + size),
                 capacity - size, NULL, 0, info, &flags);

        if (-1 == ret)
          fatal ("sctp_recvmsg");

        if (0 == ret)
          break;

        size += ret;
        if (! (flags & MSG_EOR) && size == capacity)
          {
            capacity *= 2;
            buff = new char[capacity + 1];
            swap_ptr (mmsg, buff);
            delete buff;
          }
      } while (! (flags & MSG_EOR));

    mmsg[size] = '\0';
    *msg = mmsg;
    *msg_size = size;

    return ;
  }
