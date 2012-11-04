// Copyright 2012 Google Inc. All Rights Reserved.
// Author: onufry@google.com (Onufry Wojtaszczyk)
//
// Spped for various struct sizes.

#include <cstdio>
#include <sys/time.h>

#include "./generic_benchmark.h"

static const int N = 1000;

template<int S>
struct Varied {
 public:
  Varied() {}

  explicit Varied(int x) {
    for (int i = 0; i < S; ++i) {
      tab[i] = x ^ i;
    }
  }

  int tab[S];
};

template<int S>
class VariedBenchmark : public Benchmark {
 public:
  virtual void Setup() {
    // Some rather random values in the table;
    timeval current_time;
    gettimeofday(&current_time, NULL);
    table_[0] = Varied<S>(current_time.tv_usec);
    for (int i = 1; i < N; ++i) {
      table_[i] = Varied<S>((current_time.tv_sec ^ table_[i-1].tab[0]) + i);
    }
  }

  virtual void Run(long long int number_of_repetitions) {
    for (long long int run = 0; run < number_of_repetitions; ++run) {
      int result = 0;
      for (int i = 0; i < N - 1; ++i) {
        result += table_[i].tab[0] + table_[i].tab[S-1];
      }
      dummy_ += result;
    }
  }

 private:
  Varied<S> table_[N + 3];
};

void DoBenchmark(Benchmark* benchmark, const char* description) {
  static const double kThousand = 1000;

  std::pair<int, long long int> result = benchmark->RunBenchmark();

  double added_fields = static_cast<double>(result.second) * N / kThousand;
  printf("Throughput for %s: % 8.2fMB per second.\n",
         description, added_fields * sizeof(int) / result.first);
  delete benchmark;
}

void DoBenchmark(Benchmark* benchmark, int N) {
  char desc[20];
  sprintf(desc, "Size of struct: %d", N);
  DoBenchmark(benchmark, desc);
}

int main() {
  DoBenchmark(new VariedBenchmark<2>(), sizeof(Varied<2>));
  DoBenchmark(new VariedBenchmark<3>(), sizeof(Varied<3>));
  DoBenchmark(new VariedBenchmark<4>(), sizeof(Varied<4>));
  DoBenchmark(new VariedBenchmark<5>(), sizeof(Varied<5>));
  DoBenchmark(new VariedBenchmark<6>(), sizeof(Varied<6>));
  DoBenchmark(new VariedBenchmark<7>(), sizeof(Varied<7>));
  DoBenchmark(new VariedBenchmark<8>(), sizeof(Varied<8>));
  DoBenchmark(new VariedBenchmark<9>(), sizeof(Varied<9>));
  DoBenchmark(new VariedBenchmark<10>(), sizeof(Varied<10>));
  DoBenchmark(new VariedBenchmark<11>(), sizeof(Varied<11>));
  DoBenchmark(new VariedBenchmark<12>(), sizeof(Varied<12>));
  DoBenchmark(new VariedBenchmark<13>(), sizeof(Varied<13>));
  DoBenchmark(new VariedBenchmark<14>(), sizeof(Varied<14>));
  DoBenchmark(new VariedBenchmark<15>(), sizeof(Varied<15>));
  DoBenchmark(new VariedBenchmark<16>(), sizeof(Varied<16>));
  DoBenchmark(new VariedBenchmark<17>(), sizeof(Varied<17>));
  return 0;
}
