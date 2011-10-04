#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include "err.h"

extern int sys_nerr;

void syserr(const char *fmt, ...)
{
    va_list fmt_args;
    int err;

    fprintf(stderr, "ERROR: ");
    err = errno;

    va_start(fmt_args, fmt);
    if (vfprintf(stderr, fmt, fmt_args) < 0) {
	fprintf(stderr, " (also error in syserr) ");
    };
    va_end(fmt_args);
    fprintf(stderr, " (%d; %s)\n", err, strerror(errno));
    exit(EXIT_FAILURE);
}

void fatal(const char *fmt, ...)
{
    va_list fmt_args;

    fprintf(stderr, "ERROR: ");

    va_start(fmt_args, fmt);
    if (vfprintf(stderr, fmt, fmt_args) < 0) {
	fprintf(stderr, " (also error in fatal) ");
    };
    va_end(fmt_args);

    fprintf(stderr, "\n");
    exit(EXIT_FAILURE);
}
