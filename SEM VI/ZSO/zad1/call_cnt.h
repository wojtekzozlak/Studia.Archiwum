#ifndef CALL_CNT_H
#define CALL_CNT_H

#include <sys/cdefs.h>
#include <stdio.h>
#include <sys/types.h>

struct call_cnt;

__BEGIN_DECLS

int intercept(struct call_cnt ** desc, char const * lib_name);
int stop_intercepting(struct call_cnt * desc);
int release_stats(struct call_cnt * desc);
int print_stats_to_stream(FILE * stream, struct call_cnt * desc);
ssize_t get_num_intern_calls(struct call_cnt * desc);
ssize_t get_num_extern_calls(struct call_cnt * desc);

__END_DECLS

#endif /* CALL_CNT_H */
