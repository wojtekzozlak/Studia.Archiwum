// Copyright 2012 Google Inc. All Rights Reserved.
// Author: onufry@google.com (Onufry Wojtaszczyk)

#include <cstdio>
#include <sys/time.h>

#include "./generic_benchmark.h"

namespace {

typedef long long int int64;

// Returns the current timestamp, in milliseconds since epoch.
inline int GetTime() {
  timeval current_time;
  gettimeofday(&current_time, NULL);
  return current_time.tv_sec * 1000 + current_time.tv_usec / 1000;
}

}  // namespace

std::pair<int, long long int> Benchmark::RunBenchmark(int expected_runtime_ms) {
  int64 current_runs = 1;
  int current_time = 0;
  int failed_runs = 0;
  while (current_time < 2 * expected_runtime_ms / 3) {
    current_runs *= 2LL;
    Setup();
    int start_time = GetTime();
    Run(current_runs);
    int new_time = GetTime() - start_time;
    if (new_time <= current_time) {
      ++failed_runs;
    } else {
      current_time = new_time;
    }
    if (failed_runs > 30) {
      // Something went wrong - we increase the number of runs, and the runtime
      // decreased. Output an error message and leave.
      printf("Ooops. Looks like runtime is independent of number of runs.\n");
      return std::make_pair(1, 1);
    }
    // An operation intended only to use the dummy_ variable.
    if (dummy_ == 1235) {
      printf("\n");
    }
  }
  return std::make_pair(current_time, current_runs);
}
