// Copyright 2012 Google Inc. All Rights Reserved.
// Author: onufry@google.com (Onufry Wojtaszczyk)
//
// Benchmarks the costs of calling a virtual function, an non-inlined function,
// an inline function and a hard-coded operation.
// We will use addition of a constant to the argument as the internals of the
// function benchmarked in all the cases.

#include <cstdio>
#include <sys/time.h>
#include <utility>

#include "./generic_benchmark.h"

// Benchmark - how much time does it take to add N integers?
class SampleBenchmark : public Benchmark {
 public:
  SampleBenchmark(int table_size)
      : N(table_size),
        table_(new int[table_size]) {}

  virtual ~SampleBenchmark() { delete[] table_; }

  virtual void Setup() {
    // Set up some rather random values in the table.
    timeval current_time;
    gettimeofday(&current_time, NULL);
    table_[0] = current_time.tv_sec;
    for (int i = 1; i < N; ++i) {
      table_[i] = table_[i - 1] ^ ((N + 7) * i);
    }
  }

  virtual void Run(long long int number_of_repetitions) {
    for (long long int run = 0; run < number_of_repetitions; ++run) {
      int result = 0;
      // We do the "& run" so the runs differ.
      for (int i = 0; i < N; ++i) result += table_[i & run];
      // So the calculation does not get compiled away.
      dummy_ += result;
    }
  }

 private:
  int N;
  int* table_;
};

int main() {
  static const double kMillion = 1000000;
  static const double kThousand = 1000;

  for (int N = 1 << 10; N < (1 << 20); N *= 2) {
    Benchmark* benchmark(new SampleBenchmark(N));
    std::pair<int, long long int> result = benchmark->RunBenchmark();

    double added_fields =
        static_cast<double>(result.second) * static_cast<double>(N) / kMillion;
    printf("Throughput for % 8d ints: % 8.2fM per second.\n",
           N, added_fields / (result.first / kThousand));
    delete benchmark;
  }
  return 0;
}
