/*
 * Autor: Osowski Marcin
 * nr indeksu: 292682
 *
 * Pierwsze zadanie z Zaawansowanych Systemów Operacyjnych (2012)
 *
 */

#include "libtest.h"
#include "err.h"
#include <stdlib.h>

/***************** Prologue common to all test cases *********************/
typedef volatile uint32_t atomic_t;

static atomic_t intern_calls = 0;
static atomic_t extern_calls = 0;

/* Atomically increases variable of atomic_t type */
inline static void atomic_inc(atomic_t* x)
{
    asm ("lock incl %0"
        : /* no output */
        : "m"(*x)
        );
}

#define INT_CALL(x) (atomic_inc(&intern_calls), (x))
#define EXT_CALL(x) (atomic_inc(&extern_calls), (x))

uint32_t expected_extern_calls()
{
    return extern_calls;
}

uint32_t expected_intern_calls()
{
    return intern_calls;
}


#define EP2(NUM)           ENTRY_POINT_IN_USE ## NUM
#define EP1(NUM)           EP2(NUM)
#define ENTRY_POINT_IN_USE EP1(TEST_CASE)

/******************** Actual testing code starts here **********************/

#if TEST_CASE == 1 || TEST_CASE == 101
/* Simple case, just a few calls. */
void my_malloc_free()
{
    EXT_CALL(free(EXT_CALL(malloc(1))));
}
void ENTRY_POINT_IN_USE()
{
    INT_CALL(my_malloc_free());
    INT_CALL(my_malloc_free());
    INT_CALL(my_malloc_free());
}

#elif TEST_CASE == 2 || TEST_CASE == 102
/* More complicated one: constnant, defineable number of calls */
#define INT_CALLS_COUNT (1000*1000)
void nop_function()
{
}

void ENTRY_POINT_IN_USE()
{
    int i;
    for(i=0; i<INT_CALLS_COUNT - 1; ++i)
        INT_CALL(nop_function());
}

#elif TEST_CASE == 3 || TEST_CASE == 103
/* Computing fibonacci the direct way. */
int fib(int x)
{
    if(x <= 1)
        return x;
    return INT_CALL(fib(x - 1)) + INT_CALL(fib(x - 2));
}
void ENTRY_POINT_IN_USE()
{
    INT_CALL(fib(30));
}

#elif TEST_CASE == 4 || TEST_CASE == 104
/* Randomized calls. */
void foobar1();
void foobar2();
void foobar1()
{
    if(EXT_CALL(rand()) % 200 != 0)
        INT_CALL(foobar2());
}
void foobar2()
{
    if(EXT_CALL(rand()) % 200 != 0)
        INT_CALL(foobar1());
}
void ENTRY_POINT_IN_USE()
{
    INT_CALL(foobar2());
}

#elif TEST_CASE == 5 || TEST_CASE == 105
/* Calling stdlib qsort with callback from this library */
int compare (const void * a, const void * b)
{
      return ( *(int*)a - *(int*)b );
}
int compare_wrap (const void * a, const void * b)
{
    return INT_CALL(compare(a,b));
}
void ENTRY_POINT_IN_USE()
{
    size_t size = 5000 + EXT_CALL(rand()) % 5000;
    int tab[size];
    int i;
    for(i=0; i<size; ++i)
        tab[i] = EXT_CALL(rand());
    EXT_CALL(qsort(tab, size, sizeof(int), compare_wrap));
}

#elif TEST_CASE == 6 || TEST_CASE == 106
/* Generating LOTS of symbols */
#include "gazillions_foobars.h"

void ENTRY_POINT_IN_USE()
{
    INT_CALL(gazillions_foobars());
}

#elif TEST_CASE == 201 || TEST_CASE == 202
/* libcall_cnt::intercept() shall be called twice */

void ENTRY_POINT_IN_USE()
{
    int i;
    for(i=0; i<100; ++i){
        EXT_CALL(free(EXT_CALL(malloc(1))));
    }
}
#else
#error
#error Unknown test case
#error
#endif

/********************* GENERIC ENTRIES *********************/
#if TEST_CASE < 100
/* Single-threaded entry. */
void entry_point()
{
    EXT_CALL(debug(8, "Test entry point called"));
    INT_CALL(ENTRY_POINT_IN_USE());
    EXT_CALL(debug(8, "Test entry point quitting"));
}
#elif TEST_CASE < 200
/* Multi-threaded entry. */
void* entry_point(void* foo)
{
    foo = foo;
    EXT_CALL(debug(8, "Test entry point called"));
    INT_CALL(ENTRY_POINT_IN_USE());
    EXT_CALL(debug(8, "Test entry point quitting"));
    return NULL;
}
#else
/* Special test cases */
void entry_point()
{
    INT_CALL(ENTRY_POINT_IN_USE());
}
#endif

