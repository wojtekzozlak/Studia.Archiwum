#ifndef _PAGESIM_H_
#define _PAGESIM_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>
#include <pthread.h>
#include <aio.h>
#include "page.h"
#include "strategy.h"

// errors
#define PSIM_EBALL 1
#define PSIM_EIO   2
#define PSIM_EOOFB 3
#define PSIM_ENOTR 4
#define PSIM_ECOND 5
#define PSIM_ETHRE 6
#define PSIM_ELOCK 7
#define PSIM_EUNLO 8
#define PSIM_ECLEA 9

// states
#define PSIM_EMPTY   0 // page state
#define PSIM_READY   1 // page/lib state
#define PSIM_IN_USE  2 // page state
#define PSIM_DOWN    3 // lib state
#define PSIM_CLOSING 4 // lib state
#define PSIM_UNKNOWN 5 // lib state


typedef void (*pagesim_callback)(int op, int arg1, int arg2);


int page_sim_init(unsigned page_size, unsigned mem_size,
                  unsigned addr_space_size, unsigned max_concurrent_operations,
                  pagesim_callback callback);
/* Function inits page simulator.                             *
 *                                                            *
 * RETURN VALUE:                                              *
 *    0   if successful                                       *
 *   -1   if error occurred (sets `errno`)                    *
 *                                                            *
 * ERRNO VALUES:                                              *
 *  PSIM_EBALL     memory allocation failed                *
 *  PSIM_EIO      address space file is corrupted         *
 *  PSIM_ETHRE    thread synchronization function failed  *
 *                                                            */
 
 
int page_sim_set(unsigned a, uint8_t v);
int page_sim_get(unsigned a, uint8_t *v);
/* Function page_sim_set() writes byte number [a] from [v]   *
 * into the memory.                                          *
 *                                                           *
 *                                                           *
 * Function page_sim_get() reads byte number [a] from the    *
 * simulated memory and writes it into [v]                   *
 *                                                           *
 * RETURN VALUE:                                             *
 *   0  success                                              *
 *  -1  failure (sets errno)                                 *
 *                                                           *
 * POSSIBLE ERRORS:                                          *
 *   PSIM_ECOND    cond operation failure                  *
 *   PSIM_ELOCK      mutex lock failure                      *
 *   PSIM_EUNLO    mutex unlock failure                    *
 *   PSIM_EIO    aio operation failure                   *
 *                                                           *
 * If PSIM_ECOND, PSIM_ELOCK, PSIM_EUNLO error occured   *
 * the library is in unknown state. You can run              *
 * page_sim_end() and if it is successful, you can still     *
 * work with the library. If not so - something very bad     *
 * happened and it is permanently corrupted.                 *
 * It is also possible, that error was caused by             *
 * page_sim_end() operation executed in the middle of        *
 * get/set operation.                                        *
 *                                                           *
 * The PSIM_EIO means, that get/set operation failed     *
 * but the library is in the safe state, so the next         *
 * may be successful.                                        */


int page_sim_end();
/* Function ends simulation by running resource cleanup. Can fail if any   *
 * get/set operation is in progress, leaving the library in unknown state. *
 * Failure prevents memory release because of get/set operations which can *
 * be in progress, so if you're positive, that all of them has ended, you  *
 * should run page_sim_mem_cleanup() separatly.                            *
 *                                                                         *
 * RETURN VALUE:                                                           *
 *   0              success                                                *
 *   PSIM_ECLEA  the cond/mutex cleanup failed and the library is in    *
 *                      a unknown state                                    *
 *   PSIM_EIO   memory file close/unlink failed and the library is in  *
 *                      a unknown state                                    */

#endif
