#ifndef _PAGE_SIM_PAGE_H_
#define _PAGE_SIM_PAGE_H_

#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>

typedef struct page {
    unsigned p_num;      /* page number                       */
    unsigned f_num;      /* frame number (-1 when not loaded) */
    unsigned hits;       /* successful hit during frame life  */
    bool mod;            /* modification bit (byte...)        */
    uint8_t state;       /* page state                        */
    uint8_t *bytes;      /* page content                      */
    pthread_cond_t cond; /* page cond                         */
    struct page *next;   /* list next pointer                 */
    struct page *prev;   /* list previous pointer             */
} page;

#endif
