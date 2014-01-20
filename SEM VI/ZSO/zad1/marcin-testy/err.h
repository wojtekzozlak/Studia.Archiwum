/*
 * Autor: Osowski Marcin
 * nr indeksu: 292682
 *
 * Pierwsze zadanie z Zaawansowanych Systemów Operacyjnych (2012)
 *
 */

#ifndef _ERR_H_
#define _ERR_H_

__attribute__((noreturn)) void syserr(const char *fmt, ...);
__attribute__((noreturn)) void fatal(const char *fmt, ...);
void debug(int debuglevel, const char *fmt, ...);

#define ASSERT_SYSCALL(x) \
	if((x) == -1) syserr("Error in file %s, line %d", __FILE__, __LINE__)

#define ASSERT_PTHREAD(x) \
{ \
    int aVeryLongIDThatIsNotSupposedToAppear384759 = (x); \
    if(aVeryLongIDThatIsNotSupposedToAppear384759 != 0) \
        fatal("Pthreads %d error in file %s, line %d", \
           aVeryLongIDThatIsNotSupposedToAppear384759, __FILE__, __LINE__); \
}

#define ASSERT_MALLOC(x) \
    if(!(x)) fatal("Unsuccessful malloc in file %s, line %d", __FILE__, __LINE__);

#endif /* _ERR_H_ */
