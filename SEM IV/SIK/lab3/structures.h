#include <list>
using namespace std;

#define OK      0
#define NOT_YET 1
#define ERR     2
#define JUMP    3

#define NOW  0
#define READ 1

#define MAX_TRIES 5


class
Handler;

struct
Client
  {
    int fd;
    event *ev;
    list<Handler*> recipe;
    list<Handler*>::iterator cur_handler;
    uint16_t msg_code;
    uint32_t id;
    char *c_id;
    uint32_t msg;
    char *c_msg;
    int nr;
    set<Client*> *clients;
    map<string, int> *ids;

    Client () : c_id (NULL), c_msg (NULL) {};
  };

class
Handler
  {
    public:
      int execution;

      virtual int
      handle (Client &str) = 0;
      virtual ~Handler() {};
  };


class
Reader : public Handler
  {
    protected:
      const uint32_t *expected;
      uint32_t bytes_read;
      char *result;
      char **rresult;

      virtual void
      finalize () {
         *(result + bytes_read) = '\0';
      };

    public:
      Reader (const uint32_t *expected, char *result) :
        expected (expected), bytes_read (0), result (result), rresult (NULL)
        { execution = READ; };

      Reader (const uint32_t *expected, char **rresult) :
        expected (expected), bytes_read (0), result (NULL), rresult (rresult)
        { execution = READ; };

      virtual
      ~Reader () {};

      virtual int
      handle (Client &str)
        {
          int r;
          if (*expected - bytes_read == 0)
            return ERR;
          if (rresult != NULL)
            result = *rresult;

          r = read (str.fd, (result + sizeof (char) * bytes_read),
                                                      *expected - bytes_read);
          if (r <= 0)
            return ERR;

          bytes_read += r;
          if (bytes_read == *expected)
            {
              finalize ();
              bytes_read = 0;
              return OK;
            }
          else
            return NOT_YET;
        }
  };


class
Reader32 : public Reader
  {
    private:
      uint32_t size;

    protected:
      virtual void
      finalize ()
        { *(uint32_t*) result = ntohl ( *(uint32_t*) result); }

    public:
      Reader32 (char *result) : Reader (&size, result), size (4) {};
      ~Reader32 () {};
  };

class
Reader16 : public Reader
  {
    private:
      uint32_t size;

    protected:
      virtual void
      finalize()
        { *(uint16_t*) result = ntohs (*(uint16_t*) result); }

    public:
      Reader16 (char *result) : Reader (&size, result), size (2) {};
      ~Reader16 () {};
  };

class
CodeChecker : public Handler
  {
    private:
      uint16_t code;
      int tries;

    public:
      CodeChecker (uint16_t code) : code (code), tries (0) { execution = NOW; };
      ~CodeChecker () {};

      int
      handle (Client &str)
        {
          if (str.msg_code == code)
            return OK;
          else if (tries == MAX_TRIES)
            return ERR;
          str.cur_handler--;
          tries++;
          return JUMP;
        }
  };

class
Writer : public Handler
  {
    protected:
      char *msg;
      int msg_len;

      virtual void
      prepare_msg (Client &str) = 0;

    public:
      Writer () : msg (NULL), msg_len (0) { execution = NOW; };
      virtual
      ~Writer () {};

      int
      handle (Client &str)
        {
          int wrote = 0, result;
          prepare_msg (str);
          while(msg_len - wrote != 0)
            {
              result = write (str.fd, msg + wrote * sizeof(char),
                              msg_len - wrote);
              if (result == -1)
                return ERR;
              wrote += result;
            }
	  if(msg != NULL)
	    {
              delete msg;
              msg = NULL;
            }
          return OK;
        }
  };

class
SendPlace : public Writer
  {
    protected:
      void
      prepare_msg (Client &str)
        {
          uint32_t place = htonl (str.nr);
          uint16_t place_code = htons (1);
          msg = new char[6];
          memcpy (msg + 2, &place, 4);
          memcpy (msg, &place_code, 2);
          msg_len = 6;
        }

    public:
      SendPlace () {};
      ~SendPlace () {};
  };

class
SendConfirmation : public Writer
 {
    protected:
      void
      prepare_msg (Client &str)
        {
          uint16_t conf_code = htons (3);
          uint32_t len;
          uint32_t num;
          char buf[256];
          map<string, int>::iterator it;
          string id (str.c_id);

          it = str.ids->find (id);
          if (it == str.ids->end())
            {
              str.ids->insert( pair<string, int> (id, 1) );
              num = 1;
            }
          else
            {
              num = ++it->second;
            }
          sprintf(buf, "wiadomość nr %d z tego id", num);
          len = htonl (strlen(buf));
          msg = new char[6 + ntohl (len)];
          memcpy (msg, &conf_code, 2);
          memcpy (msg + 2, &len, 4);
          memcpy (msg + 6, buf, ntohl (len));
          msg_len = 6 + ntohl (len);
        }

    public:
      SendConfirmation () {};
      ~SendConfirmation () {};
 };

class
QueueWait : public Handler
  {
    private:
      int tries;

    public:
      QueueWait () : tries(0) { execution = READ; };

      int
      handle (Client &str)
        {
          if (str.nr == 1)
            return OK;
          else if (tries == MAX_TRIES)
            return ERR;
          str.cur_handler--;
          tries++;
          return JUMP;
        }
  };

class
Allocator : public Handler
  {
    private:
      char **target;
      uint32_t *size;

    public:
      Allocator (char **target, uint32_t *size) : target (target), size (size)
        { execution = NOW; }

      int
      handle (Client &str)
        { *target = new char[*size + 1]; return OK; }
  };


class
Finalizer : public Handler
  {
    public:
      Finalizer() { execution = NOW; }

      int
      handle (Client &str)
        {
          SendPlace sender;
          set<Client*>::iterator it;

          cout << "Message received:"<< endl;
          cout << ">> " << str.c_id << ": " << str.c_msg << endl;

          for(it = str.clients->begin (); it != str.clients->end (); it++)
            {
              (*it)->nr--;
              if ((*it)->nr != 0)
                sender.handle (**it);
            }
          return OK;
        }
  };
