/*
 * Autor: Osowski Marcin
 * nr indeksu: 292682
 *
 * Pierwsze zadanie z Zaawansowanych Systemów Operacyjnych (2012)
 *
 */

#ifndef _LIBTEST_H_
#define _LIBTEST_H_

#include <inttypes.h>

#ifndef TEST_CASE
#error
#error "Please choose a test case."
#error
#endif

uint32_t expected_extern_calls();
uint32_t expected_intern_calls();

#if TEST_CASE < 100 || TEST_CASE >= 200
void entry_point();
#elif TEST_CASE < 200
void* entry_point(void* foo);
#endif

#endif /* _LIBTEST_H_ */

