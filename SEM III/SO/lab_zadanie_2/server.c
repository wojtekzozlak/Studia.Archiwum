#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <pthread.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include "common.h"
#include "err.h"

#ifdef DEBUG
const bool debug = true;
#else
const bool debug = false;
#endif

struct ThreadDataStr
{
  long handler;       /* handler id                               */
  pthread_t thread;   /* handler trhead                           */
  long *res_msg;      /* resources msg array                      */
  bool has_res_mutex; /* if handler have a lock on the res_mutex  */
};
typedef struct ThreadDataStr ThreadData;

struct OperatorDataStr
{
  ThreadData **threads;           /* threads array                  */
  long threads_size, threads_num; /* threads array controls         */
  pthread_mutex_t threads_mutex;  /* mutex for threads array        */
  pthread_t thread_watchman;      /* watchman thread                */
  pthread_t thread_gravedigger;   /* gravedigger thread             */
  long *handlers_free;            /* recycled handlers id array     */
  long handlers_free_num, handlers_free_size; /* controls for above */
  long next_handler;              /* next unused handler id         */
  bool the_end;                   /* program termination flag       */
};
typedef struct OperatorDataStr OperatorData;

int id_rec, id_hand_in,    /* queues IDs                        */
    id_hand_out, id_grave;
int N;                     /* number of resources               */
bool *res_free;            /* table of resources availablity    */
pthread_mutex_t res_mutex; /* mutex for res_free array          */ 
pthread_cond_t *res_cond;  /* array of conditions for resources */

void clean_queues();
/* Procedure destroys opened IPC queues.                         */

void check(int ret, const char *str)
/* Procedure checks if [ret] value is equal to zero. If not so   *
 * terminates the program.                                       */
{
  if(ret != 0)
  {
    clean_queues();
    printf("%d\n", ret);
    syserr(str);
  }
  return ;
}

void safe_syserr(const char *str)
/* Simple wrapper for the `syserr` function which cleans queues before *
 * termination.                                                        */
{
  clean_queues();
  syserr(str);
  return ;
}

int safe_msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg)
/* Simple wrapper for the system `msgsnd` function, which terminates *
 * program execution using `safe_syserr` if an error is encountered. */
{
  int result;
  if((result = msgsnd(msqid, msgp, msgsz, msgflg)) == -1)
    safe_syserr("msgsnd");
  return result;
}

ssize_t safe_msgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp,
                    int msgflg)
/* Simple wrapper for the system `msgrcv` function, which terminates *
 * program execution using `safe_syserr` if an error is encountered. */
{
  ssize_t result;
  if((result = msgrcv(msqid, msgp, msgsz, msgtyp, msgflg)) == -1)
    safe_syserr("msgrcv");
  return result;
}


void safe_fatal_msg_expected(char *str, long comm)
/* Simple wrapper for the `fatal_msg_expected` function, which cleans *
 * queues first.                                                      */
{
  clean_queues();
  fatal_msg_expected(str, comm);
  return ;
}

int open_server_queue(key_t key)
/* Simple wrapper for the system `msgget` function, which opens an IPC     *
 * queue in server mode using a [key] key and terminates program execution *
 * if an error is encountered.                                             */
{
  int id;
  if((id = msgget(key, 0666 | IPC_CREAT | IPC_EXCL)) == -1)
  {
    if(errno == EEXIST) fatal("Server is already running or the IPC \
queue hasn't been properly destroyed.\n\
Try \"./cleaner\" to clean up queue data.");
    else safe_syserr("msgget");
    return -1;
  }
  else return id;
}

void aquire_resource(int res, ThreadData *data)
/* Procedure aquires a [res] resource for a handler specified in the [data]. *
 * In order to achieve this it tries to get a lock on the [res_mutex],       *
 * and if necessary waits on a proper condition for the resource relase.     *
 *                                                                           *
 * MUTABLE PARAMETERS: data                                                  */
{
  if(debug) fprintf(stderr, "handler %ld: want a resource %d\n",
                    data->handler, res);
  check(pthread_mutex_lock(&res_mutex), "pthread_mutex_lock");
  data->has_res_mutex = true;
  while(!res_free[res])
    check(pthread_cond_wait(&res_cond[res], &res_mutex), "pthread_cond_wait");
  if(debug) fprintf(stderr, "handler %ld: resource %d aquired\n",
                    data->handler, res);
  res_free[res] = false;
  data->has_res_mutex = false;
  check(pthread_mutex_unlock(&res_mutex), "pthread_mutex_unlock");
  return ;
}

void free_resource(int res, ThreadData *data)
/* Procedures relases a [res] resource and sends a signal to threads which *
 * are waiting on a proper condition                                       *
 *                                                                         *
 * MUTABLE PARAMETERS: data                                                */
{
  check(pthread_mutex_lock(&res_mutex), "pthread_mutex_lock");
  if(debug) fprintf(stderr, "handler %ld: the %d resource returned\n",
                    data->handler, res);
  res_free[res] = true;
  check(pthread_cond_signal(&res_cond[res]), "pthread_cond_singal");
  check(pthread_mutex_unlock(&res_mutex), "pthread_mutex_unlock");
  return ;
}

int get_id(OperatorData *data)
/* Function returns a unique ID for the handler. Unique means `unique during *
 * the handlers life`. After handlers death his ID can be recycled.          *
 *                                                                           *
 * MUTABLE PARAMETERS: data                                                  */
{
  int id;
  if(data->handlers_free_num > 0)
  {
    id = data->handlers_free[data->handlers_free_num-1];
    data->handlers_free_num--;
    if(data->handlers_free_size > 2 &&
      data->handlers_free_num < data->handlers_free_size / 4)
    {
      data->handlers_free_size /= 2;
      data->handlers_free =
        realloc(data->handlers_free, data->handlers_free_size * sizeof(long));
      if(data->handlers_free == NULL) safe_syserr("realloc");
    }
  }
  else
  {
    id = data->next_handler;
    data->next_handler++;
  }
  return  id;
}

void return_id(int id, OperatorData *data)
/* Procedure recycles [id] ID and puts it in a [data->handlers_free] array. *
 *                                                                          *
 * MUTABLE PARAMETERS: data                                                 */
{
  if(data->handlers_free_num >= data->handlers_free_size)
  {
    data->handlers_free_size *= 2;
    data->handlers_free =
      realloc(data->handlers_free, data->handlers_free_size * sizeof(long));
    if(data->handlers_free == NULL)
      safe_syserr("realloc");
  }
  data->handlers_free[data->handlers_free_num] = id;
  data->handlers_free_num++;
  return ;
}

void handler_queue_cleanup(const ThreadData *data)
/* Procedure cleans up queues from handler's (and its client) messages. */
{
  Message msg;
  while(msgrcv(id_hand_out, &msg, MESSAGE_SIZE, data->handler,
               IPC_NOWAIT | MSG_NOERROR) != -1);
  if(errno != ENOMSG)
    safe_syserr("msgrcv");
  while(msgrcv(id_hand_in, &msg, MESSAGE_SIZE, data->handler,
               IPC_NOWAIT | MSG_NOERROR) != -1);
  if(errno != ENOMSG)
    safe_syserr("msgrcv");
  return ;
}

void handler_cleanup(void *arg)
/* Cleanup function for a handler. Cleans queues, frees dynamic memory *
 * and (if necessary) unlocks [res_mutex]                              *
 *                                                                     *
 * MUTABLE PARAMETERS: arg                                             */
{
  ThreadData *data = (ThreadData*)arg;
  if(debug) fprintf(stderr, "handler %ld: cleanup\n", data->handler);
  handler_queue_cleanup(data);
  if(data->res_msg != NULL)
    free(data->res_msg);
  if(data->has_res_mutex)
  {
    if(debug) fprintf(stderr, "handler %ld: unlocking the resources mutex\n",
                      data->handler);
    check(pthread_mutex_unlock(&res_mutex), "pthread_mutex_unlock");
  }
  return ;
}

void handler_join_me(const ThreadData *data)
/* Procedure sends JOIN_ME message to the gravedigger thread and terminates *
 * the handler thread.                                                      */
{
  Message msg;
  handler_queue_cleanup(data);
  set_message(&msg, 1, JOIN_ME, data->handler);
  safe_msgsnd(id_grave, &msg, MESSAGE_SIZE, 0);
  if(debug) fprintf(stderr, "handler %ld: going to die\n", data->handler);
  pthread_exit(0);
  return ;
}

void handler_send_id(long id)
/* Procedure sends handler id [id] to the client. */
{
  Message msg;
  if(debug) fprintf(stderr, "handler %ld: sending id to the client\n", id);
  set_message(&msg, ID_REPO, HANDLER_OFFER, id);
  safe_msgsnd(id_hand_out, &msg, MESSAGE_SIZE, 0);
  return ;
}

int handler_wait_for_request(ThreadData *data)
/* Function waits for the client's request and than returns the number of   *
 * resources which client will send in the next message. If the RESIGNATION *
 * is encountered in place of the WANT_RESOURCES, function terminates the   *
 * handler.                                                                 */
{
  Message msg;
  if(debug) fprintf(stderr, "handler %ld: waiting for a request\n",
                    data->handler);
  safe_msgrcv(id_hand_in, &msg, MESSAGE_SIZE, data->handler, 0);
  if(msg.comm == RESIGNATION)
    handler_join_me(data);
  else if(msg.comm != WANT_RESOURCES)
    safe_fatal_msg_expected("WANT_RESOURCES or RESIGNATION", msg.comm);
  return msg.value;
}

void handler_get_resources(ThreadData *data, int count)
/* Procedure fills [data->res_msg] array with resources messege received *
 * from the client. If the RESIGNATION is encountered in place of the    *
 * RESOURCES, procedure terminates the handler.                          *
 *                                                                       *
 * MUTABLE PARAMETERS: data                                              */
{
  if(debug) fprintf(stderr, "handler %ld: the client want resources\n",
                    data->handler);
  if((data->res_msg = malloc(sizeof(long) * (count + 2))) == NULL)
    safe_syserr("malloc");
  safe_msgrcv(id_hand_in, data->res_msg, sizeof(long) * (count + 1),
              data->handler, 0);
  if(data->res_msg[1] == RESIGNATION)
    handler_join_me(data);
  else if(data->res_msg[1] != RESOURCES)
    safe_fatal_msg_expected("RESOURCES or RESIGNATION", data->res_msg[1]);
  return ;
}

void handler_process_request(ThreadData *data, int count)
/* Procedure aquires resources for the client using the data in the   *
 * [data->res_msg] array, waits for the end-of-work message and frees *
 * resources.                                                         *
 * If requested resources exceeds the range of available resources    *
 * procedure sends CANT_SATISFY message to the client. If RESIGNATION *
 * message is encountered during the execution, procedure relases     *
 * aquired resources and terminates the handler thread.               *
 *                                                                    *
 * MUTABLE PARAMETERS: data                                           */
{
  int i;
  long id = data->handler;
  bool resignation = false;
  Message msg;
  for(i = 0; i < count; i++)
  {
    if(data->res_msg[2 + i] > N || data->res_msg[2 + i] < 1)
    {
      if(debug) fprintf(stderr, "handler %ld: the request can't be completed\n",
                        data->handler);
      set_message(&msg, data->handler, CANT_SATISFY, 0);
      safe_msgsnd(id_hand_out, &msg, MESSAGE_SIZE, 0);
      handler_join_me(data);
    }
  }
  qsort(data->res_msg + 2, count, sizeof(long), compare_long);
  if(debug) fprintf(stderr, "handler %ld: starting the reservation\n",
                    data->handler);
  for(i = 0; i < count; i++)
  {
    aquire_resource(data->res_msg[2 + i], data);
    if(msgrcv(id_hand_in, &msg, MESSAGE_SIZE, data->handler, IPC_NOWAIT) != -1)
    /* Not `safe_msgrcv` because of IPC_NOWAIT handling. */
    {
      if(msg.comm == RESIGNATION)
      {
        if(debug) fprintf(stderr, "handler %ld: the client has resigned\n",
                          data->handler);
        resignation = true;
        i++;
        break;
      }
      else
        safe_fatal_msg_expected("RESIGNATION or nothing", msg.comm);
    }
    else if(errno != ENOMSG)
      safe_syserr("msgrcv");
  }
  if(!resignation)
  {
    if(debug) fprintf(stderr, "handler %ld: sending the resources-aquired \
confirmation\n", id);
    set_message(&msg, id, RESOURCES_AQUIRED, 0);
    safe_msgsnd(id_hand_out, &msg, MESSAGE_SIZE, 0);
    if(debug) fprintf(stderr, "handler %ld: waiting for the clients work to \
complete\n", id);
    safe_msgrcv(id_hand_in, &msg, MESSAGE_SIZE, id, 0);
    if(msg.comm == RESIGNATION)
    {
      if(debug) fprintf(stderr, "handler %ld: the client has resigned\n", id);
    }
    else if(msg.comm != END_OF_WORK)
    {
      printf("%ld\n", msg.comm);
      safe_fatal_msg_expected("END_OF_WORK or RESIGNATION", msg.comm);
    }
  }

  i--;
  if(debug) fprintf(stderr, "handler %ld: returning resources\n", id);
  for(; i >= 0; i--)
    free_resource(data->res_msg[2 + i], data);
  if(debug) fprintf(stderr, "handler %ld: all resources returned\n", id);
  return ;
}

void *handler(void *arg)
/* Handler thread main function. Attends a client's request. *
 *                                                           *
 * MUTABLE PARAMETERS: arg                                   */
{
  ThreadData *data = (ThreadData*)arg;
  int count;

  pthread_cleanup_push(handler_cleanup, (void*)data);

  handler_send_id(data->handler);
  count = handler_wait_for_request(data);
  handler_get_resources(data, count);
  handler_process_request(data, count);

  if(debug) fprintf(stderr, "handler %ld: starting cleanup\n", data->handler);
  pthread_cleanup_pop(true);
  handler_join_me(data);
  return 0;
}

void server_init(OperatorData *data)
/* Procedure inits critical server data (queues, arrays, conditions etc.) *
 *                                                                        *
 * MUTABLE PARAMETERS: data                                               */
{
  int i;

  data->the_end = false;
  data->next_handler = 2;
  data->handlers_free_num = 0;
  data->handlers_free_size = 1;
  data->handlers_free = malloc(sizeof(long) * data->handlers_free_size);
  if(data->handlers_free == NULL)
    syserr("malloc");
  check(pthread_mutex_init(&res_mutex, NULL), "pthread_mutex_init");
  check(pthread_mutex_init(&data->threads_mutex, NULL), "pthread_mutex_init");


  if((res_cond = malloc((N + 1) * sizeof(pthread_cond_t))) == NULL)
    syserr("malloc");
  if((res_free = malloc((N + 1) * sizeof(bool))) == NULL)
    syserr("malloc");

  for(i = 1; i <= N; i++)
  {
    check(pthread_cond_init(&res_cond[i], NULL), "pthread_cond_init");
    res_free[i] = true;
  }
  data->threads_num = 0;
  data->threads_size = 2;
  if((data->threads = malloc(sizeof(ThreadData*) * data->threads_size)) == NULL)
    syserr("malloc");
  id_rec = open_server_queue(MKEY_RECEPTION);
  id_grave = open_server_queue(MKEY_GRAVEDIGGER);
  id_hand_in = open_server_queue(MKEY_HANDLER_IN);
  id_hand_out = open_server_queue(MKEY_HANDLER_OUT);
  return ;
}

void create_handler(pthread_attr_t *attr, OperatorData *data)
/* Procedure creates a handler thread for a client. *
 *                                                  *
 * MUTABLE PARAMETERS: data                         */
{
  check(pthread_mutex_lock(&data->threads_mutex), "pthread_mutex_lock");
  if(data->threads_num >= data->threads_size)
  {
    data->threads_size *= 2;
    data->threads = realloc(data->threads,
                            sizeof(ThreadData) * data->threads_size);
  }
  if((data->threads[data->threads_num] = malloc(sizeof(ThreadData))) == NULL)
    safe_syserr("malloc");
  data->threads[data->threads_num]->has_res_mutex = false;
  data->threads[data->threads_num]->res_msg = NULL;
  data->threads[data->threads_num]->handler = get_id(data);
  data->threads_num++;
  check(pthread_create(&(data->threads[data->threads_num - 1]->thread),
                       attr, handler, data->threads[data->threads_num - 1]),
        "pthread_create");
  check(pthread_mutex_unlock(&data->threads_mutex), "pthread_mutex_unlock");
  return ;
}

void *watchman(void *arg)
/* Main function for the watchman thread. Waits for the SIGINT signal, *
 * and then triggers program termination.                              *
 *                                                                     *
 * MUTABLE PARAMETERS: arg                                             */
{
  OperatorData *data = (OperatorData*)arg;
  sigset_t signals;
  int caught;
  Message msg;

  check(sigemptyset(&signals), "sigemptyset");
  check(sigaddset(&signals, SIGINT), "sigaddset");
  if(debug) fprintf(stderr, "watchman: waiting for a signal\n");
  check(sigwait(&signals, &caught), "sigwait");
  if(debug) fprintf(stderr, "watchman: termination initiated\n");
  data->the_end = true;
  set_message(&msg, RECEPTION, SHUT_DOWN, 0);
  safe_msgsnd(id_rec, &msg, MESSAGE_SIZE, 0);
  pthread_exit(0);
  return 0;
}

void clean_queues()
/* Description in the forward decalaration. */
{
  struct msqid_ds info;
  check(msgctl(id_rec, IPC_RMID, &info), "msgctl");
  check(msgctl(id_grave, IPC_RMID, &info), "msgctl");
  check(msgctl(id_hand_in, IPC_RMID, &info), "msgctl");
  check(msgctl(id_hand_out, IPC_RMID, &info), "msgctl");
}

void server_cleanup(OperatorData *data)
/* Cleans up the server data. Cancels and joins threads, destroys mutexes *
 * and conditions, closes queues etc.                                     *
 *                                                                        *
 * MUTABLE PARAMTERS: data                                                */
{
  int i;
  void *retval;

  if(debug) fprintf(stderr, "main: cleaning threads\n");
  pthread_cancel(data->thread_watchman); // no error posible
  check(pthread_join(data->thread_watchman, &retval), "pthread_join");
  pthread_cancel(data->thread_gravedigger); 
  check(pthread_join(data->thread_gravedigger, &retval), "pthread_join");
  for(i = 0; i < data->threads_num; i++)
  {
    if(debug) fprintf(stderr, "main: cleaning the %ld thread\n",
                      data->threads[i]->handler);
    pthread_cancel(data->threads[i]->thread);
    check(pthread_join(data->threads[i]->thread, &retval), "pthread_join");
    free(data->threads[i]);
  }

  if(debug) fprintf(stderr, "main: cleaning queues\n");
  clean_queues();

  if(debug) fprintf(stderr, "main: cleaning mutexes\n");
  check(pthread_mutex_destroy(&res_mutex), "pthread_mutex_destroy");
  check(pthread_mutex_destroy(&data->threads_mutex), "pthread_mutex_destroy");

  if(debug) fprintf(stderr, "main: cleaning conditions\n");
  for(i = 0; i < N + 1; i++)
    check(pthread_cond_destroy(&res_cond[i]), "pthread_cond_destroy");

  if(debug) fprintf(stderr, "main: cleaning memory\n");
  free(data->threads);
  free(res_free);
  free(data->handlers_free);
  free(res_cond);
  return ;
}

void join_and_clean_handler(int handler, OperatorData *data)
/* Procedure joins the [handler] handler, recycles its ID and cleans up *
 * its data in [data->threads] array                                    *
 *                                                                      *
 * MUTABLE PARAMETERS: data                                             */
{
  int i;
  void *retval;
  for(i = 0; i < data->threads_num; i++)
    if(data->threads[i]->handler == handler)
      break;
  check(pthread_join(data->threads[i]->thread, &retval), "pthread_join");
  if(debug) fprintf(stderr, "gravedigger: thread %ld joined\n",
                    data->threads[i]->handler);

  check(pthread_mutex_lock(&data->threads_mutex), "pthread_mutex_lock");
  return_id(handler, data); 
  free(data->threads[i]);
  data->threads[i] = data->threads[data->threads_num - 1];
  data->threads_num--;
  if(data->threads_num < data->threads_size / 4)
  {
    data->threads_size /= 2;
    data->threads = realloc(data->threads,
                            sizeof(ThreadData*) * data->threads_size);
  }
  check(pthread_mutex_unlock(&data->threads_mutex), "pthread_mutex_unlock");
  return ;
}

void *gravedigger(void *arg)
/* Main function for the gravedigger thread. Waits for the JOIN_ME message *
 * and than joins the terminated handler.                                  *
 *                                                                         *
 * MUTABLE PARAMETERS: arg                                                 */
{
  OperatorData *data = (OperatorData*)arg;
  Message msg;
  while(true)
  {
    if(debug) fprintf(stderr, "gravedigger: waiting for a thread to join\n");
    safe_msgrcv(id_grave, &msg, MESSAGE_SIZE, 0, 0);
    if(msg.comm != JOIN_ME)
      safe_fatal_msg_expected("JOIN ME", msg.comm);
    if(debug) fprintf(stderr, "gravedigger: trying to join the %ld thread\n",
                      msg.value);
    join_and_clean_handler(msg.value, data);
  }
}

int main(int argc, char *argv[]){
  Message msg;
  pthread_attr_t attr;
  OperatorData data;

  if(argc < 2)
    fatal("to few arguments");
  N = atoi(argv[1]);  

  server_init(&data);
  check(pthread_attr_init(&attr), "pthread_attr_init");

  sigset_t signal_set;
  check(sigemptyset(&signal_set), "sigemptyset");
  check(sigaddset(&signal_set, SIGINT), "sigaddset");
  check(pthread_sigmask(SIG_BLOCK, &signal_set, NULL), "pthread_sigmask");
    
  check(pthread_create(&data.thread_watchman, &attr, watchman, &data),
                       "pthread_create");
  check(pthread_create(&data.thread_gravedigger, &attr, gravedigger, &data),
                       "pthread_create");

  msg.mtype = 1;
  while(!data.the_end){
    if(debug) fprintf(stderr, "main: waiting for a client\n");
    safe_msgrcv(id_rec, &msg, MESSAGE_SIZE, RECEPTION, 0);
    if(msg.comm == SHUT_DOWN) break;
    else if(msg.comm == HANDLER_REQ)
    {
      if(debug) fprintf(stderr, "main: creating a handler for the client\n");
      create_handler(&attr, &data);
    }
    else
      safe_fatal_msg_expected("SHUT_DOWN, or HANDLER_REQ", msg.comm);
  }

  if(debug) fprintf(stderr, "main: server termination\n");
  server_cleanup(&data);

  return 0;
}
