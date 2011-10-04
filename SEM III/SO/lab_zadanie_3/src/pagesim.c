#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>
#include <pthread.h>
#include <aio.h>
#include "page.h"
#include "pagesim.h"
#include "strategy.h"

#define CHECK_NZERO(a, e) if((a) != 0) return e;
#define DEC_CHECK_NZERO(a, e) if((a) != 0) { data.current_operations--; return e; }
#define CHECK_MONE(a, e) if((a) == -1) return e;
#define RETHROW(a, v) if( (v = (a)) != 0) return v;
#define DIE_RETHROW(a, v) if( (v = (a)) != 0) { errno = v; return -1; }
#define DIE_WITH(v) { errno = v; return -1; }
#define FULL_CLEANUP_DIE_WITH(v) \
    { cleanup(); page_sim_mem_cleanup(); errno = v; return -1; }
#define DIE_NZERO(a, e) if((a) != 0) { errno = e; return -1; }
#define GET_ERROR(a, b, v) if((b = (a)) != 0) v = b;

typedef struct frame_content {
  unsigned num;   /* frame number  */
  uint8_t *bytes; /* frame content */
} frame_content;

typedef struct page_sim_data {
  unsigned page_size;                 /* size of a single page                */
  unsigned mem_size;                  /* size of an operating memory in pages */
  unsigned addr_space_size;           /* size of an addres space in pages     */
  unsigned max_concurrent_operations; /* number of concurrent operations      */
  unsigned current_operations;        /* counter of concurrent operations     */
  int file;                           /* memory file descriptor               */
  char file_name[32];                 /* memory file name                     */
  pagesim_callback callback;          /* callback function                    */
  page *pages;                        /* array of pages                       */
  page *first_page;                   /* pointer to the first frame from the  *
                                       *   frames list                        */
  page *last_page;                    /* pointer to the last frame from the   *
                                       *   frames list                        */
  uint8_t pages_count;                /* number of loaded pages               */
  uint8_t state;                      /* state of the library                 */
  pthread_cond_t create_page_cond;    /* cond for threads which are waiting   *
                                       *   for opportunity to load a page     */
  pthread_cond_t max_operations_cond; /* cond for threads which are waiting   *
                                       *   for place in the library           */
  pthread_mutex_t mutex;              /* global library mutex                 */
  frame_content *frames_repo;         /* stack of frames which are not in use */
  uint8_t *data_pool;                 /* pointer to the frames data pool      */
  unsigned frames_num;                /* number of frames in stack            */
} page_sim_data;

page_sim_data data;


void page_sim_mem_cleanup()
/* Function releases library memory. */
{
  if(data.data_pool != NULL)
  {
    free(data.data_pool);
    data.data_pool = NULL;
  }
  if(data.frames_repo != NULL)
  {
    free(data.frames_repo);
    data.frames_repo = NULL;
  }
  if(data.pages != NULL)
  {
    free(data.pages);
    data.pages = NULL;
  }
}

int cleanup()
/* Function tries to clean up resources gathered by the library.    *
 *                                                                  *
 * RETURN VALUE                                                     *
 *    0             success                                         *
 *    PSIM_EIO      unlinking or closing the memory file failed     *
 *    other         last pthread_xxx_destroy error encountered      */
{
  int ret = 0, ret2 = 0, buf, i;
  
  GET_ERROR(pthread_mutex_destroy(&data.mutex), buf, ret);
  GET_ERROR(pthread_cond_destroy(&data.max_operations_cond), buf, ret);
  GET_ERROR(pthread_cond_destroy(&data.create_page_cond), buf, ret);
  for(i = 0; i < data.addr_space_size; i++)
    GET_ERROR(pthread_cond_destroy(&data.pages[i].cond), buf, ret);

  if(data.file != -1)
  {
    if(close(data.file) != 0)
      ret2 = PSIM_EIO;
    if(unlink(data.file_name) != 0)
      ret2 = PSIM_EIO;
  }

  if(ret2 != 0)
    return PSIM_EIO;
  
  if(ret != 0)
    return PSIM_ECLEA;

  /* We're freeing memory only if operations on conds and mutex were *
   * successful. Freeing may cause segfault in get/set operation.    */
  page_sim_mem_cleanup();
  
  return 0;
}

int clear_disk_data()
/* Function clears address space file filling it with zeros  *
 *                                                           *
 * RETURN VALUE:                                             *
 *  0              success                                   *
 *  PSIM_EBALL     memory allocation failed                  *
 *  PSIM_EIO       operation on file failed                  *
 *                                                           */
{
  int i;
  uint8_t *block = (uint8_t*) malloc(sizeof(uint8_t) * data.page_size);
  
  if(block == NULL)
    return PSIM_EBALL;
  for(i = 0; i < data.page_size; i++)
    block[i] = 0;
  for(i = 0; i < data.addr_space_size; i++)
    if(write(data.file, block, data.page_size) == -1)
      return PSIM_EIO;
  free(block);
  return 0;
}

int init_page(page *page, int i)
/* Function inits empty page from pages array. *
 *                                             *
 * MUTABLE PARAMETERS: page                    *
 *                                             *
 * RETURN VALUE:                               *
 *    0             if successful              *
 *    PSIM_ECOND    if condition init failed   */
{
  page->p_num = i;
  page->f_num = -1;
  page->hits = 0;
  page->mod = false;
  page->state = PSIM_EMPTY;
  page->next = NULL;
  page->prev = NULL;
  page->bytes = NULL;
  if(pthread_cond_init(&page->cond, NULL) != 0)
    return PSIM_ECOND;
  return 0;
}

int page_sim_init(unsigned page_size, unsigned mem_size,
                  unsigned addr_space_size, unsigned max_concurrent_operations,
                  pagesim_callback callback)
/* Description in header. */
{
  
  int i;
  sprintf(data.file_name, "tmppagesimXXXXXX");
  data.page_size = page_size;
  data.mem_size = mem_size;
  data.addr_space_size = addr_space_size;
  data.max_concurrent_operations = max_concurrent_operations;
  data.callback = callback;
  data.frames_repo = NULL;
  data.data_pool = NULL;
  data.pages = NULL;
  data.file = -1;
  data.first_page = NULL;
  data.last_page = NULL;
 
  if((data.frames_repo = malloc(sizeof(frame_content) * data.mem_size)) == NULL)
    FULL_CLEANUP_DIE_WITH(PSIM_EBALL);
  if((data.data_pool =
      malloc(sizeof(uint8_t) * data.addr_space_size * data.page_size)) == NULL)
    FULL_CLEANUP_DIE_WITH(PSIM_EBALL);
  if((data.pages = malloc(sizeof(page) * addr_space_size)) == NULL)
    FULL_CLEANUP_DIE_WITH(PSIM_EBALL);
  
  data.frames_num = 0;
  for(i = 0; i < data.mem_size; i++)
  {
    data.frames_repo[i].num = i;
    data.frames_repo[i].bytes = &data.data_pool[i * data.page_size];
  }
  if((data.file = mkstemp(data.file_name)) == -1)
    FULL_CLEANUP_DIE_WITH(PSIM_EIO);
  if(pthread_cond_init(&data.create_page_cond, NULL) != 0)
    FULL_CLEANUP_DIE_WITH(PSIM_ETHRE);
  if(pthread_cond_init(&data.max_operations_cond, NULL) != 0)
    FULL_CLEANUP_DIE_WITH(PSIM_ETHRE);
  if(pthread_mutex_init(&data.mutex, NULL) != 0)
    FULL_CLEANUP_DIE_WITH(PSIM_ETHRE);
  for(i = 0; i < addr_space_size; i++)
    if(init_page(&data.pages[i], i) != 0)
      FULL_CLEANUP_DIE_WITH(PSIM_ETHRE);
  if(clear_disk_data() != 0)
    FULL_CLEANUP_DIE_WITH(PSIM_EIO);
  
  data.state = PSIM_READY;
  return 0;
}

void init_aiocb(struct aiocb *str, page *page)
/* Procedure fills aiocb structure [str] for page reading/writing *
 * using the [page].                                              *
 *                                                                *
 * MUTABLE PARAMETERS: str                                        */
{
  struct sigevent notify;
  notify.sigev_notify = SIGEV_NONE;
  str->aio_fildes = data.file;
  str->aio_offset = data.page_size * page->p_num;
  str->aio_nbytes = data.page_size;
  str->aio_buf = page->bytes;
  str->aio_sigevent = notify;
  str->aio_reqprio = 0;
  return ;
}

int write_page(page *page)
/* Function writes page into the [data.file] using the aio. *
 *                                                          *
 * RETURN VALUE:                                            *
 *  0             success                                   *
 *  PSIM_EIO      aio operation failed                      */
{
  struct aiocb request;
  struct aiocb *tab[1];
  init_aiocb(&request, page);
  tab[0] = &request;
  CHECK_MONE(aio_write(&request), PSIM_EIO);
  CHECK_MONE(aio_suspend((const struct aiocb *const *)tab, 1, NULL),
    PSIM_EIO);
  CHECK_NZERO(aio_error(&request), PSIM_EIO);
  return 0;
}

int read_page(page *page)
/* Function reads page from the [data.file] using the aio. *
 *                                                         *
 * RETURN VALUE:                                           *
 *  0             success                                  *
 *  PSIM_EIO      aio operation failed                     */
{
  struct aiocb request;
  struct aiocb *tab[1];
  init_aiocb(&request, page);  
  tab[0] = &request;
  
  CHECK_MONE(aio_read(&request), PSIM_EIO);
  CHECK_MONE(aio_suspend((const struct aiocb *const *)tab, 1, NULL),
    PSIM_EIO);
  CHECK_NZERO(aio_error(&request), PSIM_EIO);
  return 0;
}

void frames_list_push_back(page *page)
/* Procedure adds frame at the end of the frames list.      *
 *                                                          *
 * Procedure assumes, that thread have lock on [data.mutex] *
 *                                                          *
 * MUTABLE PARAMETERS: page                                 */
{
  if(data.first_page == NULL)
    data.first_page = page;
  else
  {
    data.last_page->next = page;
    page->prev = data.last_page;
  }
  data.last_page = page;
  return ;
}

void frames_list_remove(page *page)
/* Procedure removes frame from the frames list.            *
 *                                                          *
 * Procedure assumes, that thread have lock on [data.mutex] *
 *                                                          *
 * MUTABLE PARAMETERS: page                                 */
{
  if(page->prev == NULL)
    data.first_page = page->next;
  else
    page->prev->next = page->next;
  if(page->next == NULL)
    data.last_page = page->prev;
  else
    page->next->prev = page->prev;  
  page->prev = NULL;
  page->next = NULL;
  return ;
}

void stash_frame(page *page)
/* Procedure detaches frame memory from [page] and stashes frame in the *
 * [data.frames_repo].                                                  *
 *                                                                      *
 * Procedure assumes, that the thread has a lock on the [data.mutex]    *
 *                                                                      *
 * MUTABLE PARAMETERS: page                                             */
{
  data.frames_num--;
  data.frames_repo[data.frames_num].bytes = page->bytes;
  data.frames_repo[data.frames_num].num = page->f_num;
  page->bytes = NULL;
  page->f_num = -1;
  page->hits = 0;
  page->state = PSIM_EMPTY;
  return ;
}

void pop_frame(page *page)
/* Procedure attach frame memory from [data.frames_repo] into [page].   *
 *                                                                      *
 * Procedure assumes, that thread have a lock on [data.mutex]           *
 *                                                                      *
 * MUTABLE PARAMETERS: page                                             */
{
  page->bytes = data.frames_repo[data.frames_num].bytes;
  page->f_num = data.frames_repo[data.frames_num].num;
  data.frames_num++;
  return ;
}

int wait_for_place()
/* Function waits until thread can operate on memory.              *
 *                                                                 *
 * Function assumes, that the thread has lock on the [data.mutex]. *
 *                                                                 *
 * RETURN VALUE:                                                   *
 *   0              success                                        *
 *   PSIM_ECOND     operation on cond failed                       */
{
  while(data.current_operations >= data.max_concurrent_operations)
    CHECK_NZERO(pthread_cond_wait(&data.max_operations_cond, &data.mutex),
                PSIM_ECOND);
  data.current_operations++;
  return 0;
}

int load_page(unsigned num, page *my_page)
/* Function loads page number [num] from the drive to the operating memory. *
 * If there is no place for new frame, it's waiting for place or other      *
 * frame which could be removed in order to complete request                *
 *                                                                          *
 * Function assumes, that the thread has lock on the [data.mutex]           *
 *                                                                          *
 * MUTABLE PARAMETERS: my_page                                              *
 *                                                                          *
 * RETURN VALUE:                                                            *
 *   0              success                                                 *
 *   PSIM_ECOND     there was a problem with a operation on condition       *
 *   PSIM_ELOCK     mutex lock operation failed                             *
 *   PSIM_EUNLO     mutex unlock operation failed                           *
 *   IOERROR        aio operation failed                                    */
{
  int ret;
  page *to_delete = NULL;
  my_page->state = PSIM_IN_USE;
  // while we don't have place and candidate to delete - wait
  while(data.pages_count >= data.mem_size &&
      (to_delete = select_page(data.first_page)) == NULL)
    CHECK_NZERO(pthread_cond_wait(&data.create_page_cond, &data.mutex),
                PSIM_ECOND);
  if(to_delete != NULL)
  // we must remove frame before loading
  {
    to_delete->state = PSIM_IN_USE;
    frames_list_remove(to_delete);
    ret = 0;
    if(to_delete->mod);
    // frame was modified, so we have to write it back
    {
      if(data.callback != NULL)
        data.callback(2, to_delete->p_num, to_delete->f_num);
      CHECK_NZERO(wait_for_place(), PSIM_ECOND);
      DEC_CHECK_NZERO(pthread_mutex_unlock(&data.mutex), PSIM_EUNLO);
      ret = write_page(to_delete);
      DEC_CHECK_NZERO(pthread_mutex_lock(&data.mutex), PSIM_ELOCK);
      data.current_operations--;
      if(ret != 0)
      // aio_write failed, so we have to attach our frame back
      {
        frames_list_push_back(to_delete);
        // we don't control error, because we already have one
        pthread_cond_broadcast(&to_delete->cond);
        pthread_cond_signal(&data.max_operations_cond);
        to_delete->state = PSIM_READY;
        return PSIM_EIO;
      }
      // We want to try every necessary signal, before returning!
      ret = ret || pthread_cond_signal(&data.max_operations_cond);
      if(data.callback != NULL)
        data.callback(3, to_delete->p_num, to_delete->f_num);
    }
    stash_frame(to_delete);
    data.pages_count--;
    ret = ret || pthread_cond_broadcast(&to_delete->cond);
    if(ret != 0)
      return PSIM_ECOND;
  }
  data.pages_count++;

  pop_frame(my_page);
  if(data.callback != NULL)
    data.callback(4, my_page->p_num, my_page->f_num);
  CHECK_NZERO(wait_for_place(), PSIM_ECOND);
  DEC_CHECK_NZERO(pthread_mutex_unlock(&data.mutex), PSIM_EUNLO);
  ret = read_page(my_page);
  DEC_CHECK_NZERO(pthread_mutex_lock(&data.mutex), PSIM_ELOCK);
  data.current_operations--;
  if(ret != 0)
  // aio_read failed, so we have to stash frame back
  {
    stash_frame(my_page);
    data.pages_count--;
    // we don't control error, because we already have one
    pthread_cond_signal(&data.max_operations_cond);
    pthread_cond_broadcast(&data.create_page_cond);
    pthread_cond_broadcast(&my_page->cond);
    return PSIM_EIO;
  }
  my_page->state = PSIM_READY;
  frames_list_push_back(my_page);
  if(data.callback != NULL)
    data.callback(5, my_page->p_num, my_page->f_num);

  // We want to try every necessary signal, before returning!
  ret = 0;
  ret = pthread_cond_signal(&data.max_operations_cond);
  // First the ones which are waiting on the "create_page" cond!  
  ret = ret || pthread_cond_broadcast(&data.create_page_cond);
  ret = ret || pthread_cond_broadcast(&my_page->cond);
  if(ret != 0)
    return PSIM_ECOND;
  return 0;
}

int acquire_page(unsigned num, page **page)
/* Function waits until requested page is free to use and (if necessary) *
 * loads it from the hard drive.                                         *
 *                                                                       *
 * Function assumes that thread has a lock on the [data.mutex]           *
 *                                                                       *
 * MUTABLE PARAMETERS: page                                              *
 *                                                                       *
 * RETURN VALUE:                                                         *
 *   0              success                                              *
 *   PSIM_ECOND     there was a problem with a operation on condition    *
 *   PSIM_ELOCK     mutex lock operation failed                          *
 *   PSIM_EUNLO     mutex unlock operation failed                        *
 *   IOERROR        aio operation failed                                 */
{
  *page = &data.pages[num];
  int ret;
  while((*page)->state == PSIM_IN_USE)
    CHECK_NZERO(pthread_cond_wait(&((*page)->cond), &data.mutex), PSIM_ECOND);
  if((*page)->state == PSIM_EMPTY)
    RETHROW(load_page(num, *page), ret);
  (*page)->hits++;
  return 0;
}

int page_sim_get(unsigned a, uint8_t *v)
/* Description in header. */
{
  unsigned num;
  int ret = 0;
  page *my_page;
  if(data.state != PSIM_READY)
    DIE_WITH(PSIM_ENOTR);
  if(a >= (data.page_size * data.addr_space_size))
    DIE_WITH(PSIM_EOOFB);
  num = a / data.page_size;
  DIE_NZERO(pthread_mutex_lock(&data.mutex), PSIM_ELOCK);
  if(data.callback != NULL)
    data.callback(1, num, 0);

  DIE_RETHROW(acquire_page(num, &my_page), ret);
  
  *v = my_page->bytes[a - data.page_size * num];
  if(data.callback != NULL)
    data.callback(6, my_page->p_num, my_page->f_num);

  DIE_NZERO(pthread_mutex_unlock(&data.mutex), PSIM_EUNLO);
  return 0;
}

int page_sim_set(unsigned a, uint8_t v)
/* Description in header. */
{
  unsigned num;
  int ret;
  page *my_page;
  if(data.state != PSIM_READY)
    DIE_WITH(PSIM_ENOTR);
  if(a >= (data.page_size * data.addr_space_size))
    DIE_WITH(PSIM_EOOFB);
  num = a / data.page_size;
  DIE_NZERO(pthread_mutex_lock(&data.mutex), PSIM_ELOCK);
  if(data.callback != NULL)
    data.callback(1, num, 0);

  DIE_RETHROW(acquire_page(num, &my_page), ret);
    
  my_page->bytes[a - data.page_size * num] = v;
  my_page->mod = true;
  if(data.callback != NULL)
    data.callback(6, my_page->p_num, my_page->f_num);

  DIE_NZERO(pthread_mutex_unlock(&data.mutex), PSIM_EUNLO);
  return 0;
}

int page_sim_end()
/* Description in header. */
{
  int err = 0;
  if(data.state != PSIM_READY)
    DIE_WITH(PSIM_ENOTR);

  data.state = PSIM_CLOSING;
  err = cleanup();
  if(err == 0)
    data.state = PSIM_DOWN;
  else
  {
    data.state = PSIM_UNKNOWN;
    DIE_WITH(err);
    DIE_WITH(PSIM_ECLEA);
  }

  return 0;
}
