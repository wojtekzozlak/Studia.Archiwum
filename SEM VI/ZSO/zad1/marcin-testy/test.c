/*
 * Autor: Osowski Marcin
 * nr indeksu: 292682
 *
 * Pierwsze zadanie z Zaawansowanych Systemów Operacyjnych (2012)
 *
 */

#include "call_cnt.h"
#include "err.h"
#include "libtest.h"
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <pthread.h>

#define ASSERT_CALL_CNT_LIB(x)   \
    if((x)<0)                    \
        fatal("Error in test file %s, line %d", __FILE__, __LINE__)


int main()
{
    srand(time(0));
    debug(5, "Running test case %d", TEST_CASE);
    const char* TEST_LIB = "libtest.so";
    struct call_cnt* desc;
    debug(5, "Trying to intercept %s calls", TEST_LIB);
    ASSERT_CALL_CNT_LIB(intercept(&desc, TEST_LIB));

    /* Run some code here */
    #if TEST_CASE < 100
        /* One-threaded cases */
        entry_point();
    #elif TEST_CASE < 200
        /* Multi-threaded cases */
        pthread_t threads[THREADS_NUM];
        pthread_attr_t attr;
        ASSERT_PTHREAD(pthread_attr_init(&attr));
        ASSERT_PTHREAD(pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE));
        size_t i;
        for(i=0; i<THREADS_NUM; ++i){
            debug(7, "Starting thread num %d", i);
            ASSERT_PTHREAD(pthread_create(&threads[i], &attr, entry_point, NULL));
        }

        for(i=0; i<THREADS_NUM; ++i){
            ASSERT_PTHREAD(pthread_join(threads[i], NULL));
            debug(7, "Joined with thread num %d", i);
        }
        ASSERT_PTHREAD(pthread_attr_destroy(&attr));
    #elif TEST_CASE == 201
        /* double intercept() call */
        struct call_cnt *desc2;
        debug(5, "Trying to intercept %s calls again", TEST_LIB);
        ASSERT_CALL_CNT_LIB(intercept(&desc2, TEST_LIB));
        entry_point();
        debug(5, "Undoing the inner-most interception of %s", TEST_LIB);
        ASSERT_CALL_CNT_LIB(stop_intercepting(desc2));
        #if DEBUG > 0
            ASSERT_CALL_CNT_LIB(print_stats_to_stream(stdout, desc2));
        #endif
    #elif TEST_CASE == 202
        entry_point();
    #else
        fatal("No such test case");
    #endif

    debug(5, "Stopping interception of %s calls", TEST_LIB);
    ASSERT_CALL_CNT_LIB(stop_intercepting(desc));
    #if DEBUG > 0
        ASSERT_CALL_CNT_LIB(print_stats_to_stream(stdout, desc));
    #endif

    int intern_calls;
    ASSERT_CALL_CNT_LIB(intern_calls = get_num_intern_calls(desc));
    int extern_calls;
    ASSERT_CALL_CNT_LIB(extern_calls = get_num_extern_calls(desc));

    /* Perform assertions here */
    debug(5, "intern_calls = %d, expected_intern_calls = %d", intern_calls, expected_intern_calls());
    assert(intern_calls == expected_intern_calls());
    debug(5, "extern_calls = %d, expected_extern_calls = %d", extern_calls, expected_extern_calls());
    assert(extern_calls == expected_extern_calls());
    debug(5, "Assertions OK");

    #if TEST_CASE == 201
        int intern_calls2;
        ASSERT_CALL_CNT_LIB(intern_calls2 = get_num_intern_calls(desc));
        int extern_calls2;
        ASSERT_CALL_CNT_LIB(extern_calls2 = get_num_extern_calls(desc));

        /* Perform assertions here */
        debug(5, "intern_calls2 = %d, expected_intern_calls = %d", intern_calls2, expected_intern_calls());
        assert(intern_calls2 == expected_intern_calls());
        debug(5, "extern_calls2 = %d, expected_extern_calls = %d", extern_calls2, expected_extern_calls());
        assert(extern_calls2 == expected_extern_calls());
        debug(5, "Assertions #2 OK");
        debug(5, "Releasing stats #2");
        ASSERT_CALL_CNT_LIB(release_stats(desc2));
        desc2 = NULL;
    #elif TEST_CASE == 202
        debug(5, "Now calling library %s after stop_intercepting()", TEST_LIB);
        uint32_t was_expected_intern_calls = expected_intern_calls();
        uint32_t was_expected_extern_calls = expected_extern_calls();
        entry_point();
        debug(5, "intern_calls = %d, was_expected_intern_calls = %d", intern_calls, was_expected_intern_calls);
        assert(intern_calls == was_expected_intern_calls);
        debug(5, "extern_calls = %d, was_expected_extern_calls = %d", extern_calls, was_expected_extern_calls);
        assert(extern_calls == was_expected_extern_calls);
        debug(5, "Assertions #2 OK");
    #endif


    debug(5, "Releasing stats");
    ASSERT_CALL_CNT_LIB(release_stats(desc));
    desc = NULL;

    debug(5, "Test case %d complete", TEST_CASE);
    return 0;
}


