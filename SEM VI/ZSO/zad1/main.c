#include <stdlib.h>
#include "call_cnt.h"
#include "dynamic.h"

int main(){
  struct call_cnt *aaa;
  int i;
  intercept(&aaa, "libdynamic.so");

  for(i = 0; i < 1; i++) bar();

  stop_intercepting(aaa);

  print_stats_to_stream(stdout, aaa);
  printf("internal: %d\n", get_num_intern_calls(aaa));
  printf("external: %d\n", get_num_extern_calls(aaa));
  release_stats(aaa);

  return 0;
}
