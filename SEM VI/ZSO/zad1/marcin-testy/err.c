/*
 * Autor: Osowski Marcin
 * nr indeksu: 292682
 *
 * Trzecie zadanie z Systemów Operacyjnych (2010/11)
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/syscall.h>
#include <pthread.h>
#include <string.h>
#include "err.h"
#include "vt100.h"


#ifndef DEBUGLEVEL
#define DEBUGLEVEL 0
#endif

pid_t gettid()
{
    return syscall(SYS_gettid);
}

__attribute__((noreturn)) void syserr(const char *fmt, ...)
{
    typeof(errno) was_errno = errno;
    va_list fmt_args;
    fprintf(stderr, VT100_BOLD VT100_RED "[%d] ERROR: ", gettid());
    va_start(fmt_args, fmt);
    vfprintf(stderr, fmt, fmt_args);
    va_end (fmt_args);
    fprintf(stderr," (%d; %s)" VT100_NOATTR "\n", was_errno, strerror(was_errno));
    exit(1);
}

__attribute__((noreturn)) void fatal(const char *fmt, ...)
{
    va_list fmt_args;
    fprintf(stderr, VT100_BOLD VT100_RED "[%d] ERROR: ", gettid());
    va_start(fmt_args, fmt);
    vfprintf(stderr, fmt, fmt_args);
    va_end (fmt_args);
    fprintf(stderr, VT100_NOATTR "\n");
    exit(1);
}

void debug(int debuglevel, const char *fmt, ...)
{
#if DEBUGLEVEL == 0
    return;
#endif
    if(debuglevel > DEBUGLEVEL)
        return;
    if(debuglevel < 4){
        fprintf(stderr, VT100_BOLD VT100_RED "[%d] ", gettid());
    }else if(debuglevel < 7){
        fprintf(stderr, VT100_BOLD VT100_MAGENTA "[%d] ", gettid());
    }else{
        fprintf(stderr, "[%d] ", gettid());
    }
    va_list fmt_args;
    va_start(fmt_args, fmt);
    vfprintf(stderr, fmt, fmt_args);
    va_end (fmt_args);
    if(debuglevel < 7){
        fprintf(stderr, VT100_NOATTR "\n");
    }else{
        fprintf(stderr, "\n");
    }
}

