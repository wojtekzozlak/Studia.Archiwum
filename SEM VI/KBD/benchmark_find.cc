// Copyright 2012 Google Inc. All Rights Reserved.
// Author: onufry@google.com (Onufry Wojtaszczyk)
//
// Benchmarks a filter, with control dependency and data dependency. For reasons
// I don't fully know, -O6 actually worsens the performance of the SmartFinder
// code. Run with -O1.

#include <cstdio>
#include <cstdlib>
#include <utility>

#include "./generic_benchmark.h"

class Finder {
 public:
  virtual int Find(int* input, int size, int* output,
                   int selectivity) = 0;
};

class NaiveFinder : public Finder {
 public:
  virtual int Find(int* input, int size, int* output,
                   int selectivity) {
    int found = 0;
    for (int i = 0; i < size; ++i) {
      if (input[i] < selectivity) output[found++] = i;
    }
    return found;
  }
};

class SmartFinder : public Finder {
 public:
  virtual int Find(int* input, int size, int* output, int selectivity) {
    int found = 0;
    for (int i = 0; i < size; ++i) {
      output[found] = i;
      found += (input[i] > selectivity);
    }
    return found;
  }
};

static const int kTableSize = 1 << 23;
// I get segfaults trying to allocate this on the stack.
int data[kTableSize];
int result[kTableSize];

class FilterBenchmark : public Benchmark {
 public:
  // Takes ownership of the finder.
  FilterBenchmark(int selectivity, Finder* finder)
      : selectivity_(selectivity),
        finder_(finder) {
    // Set up some rather random values in the table.
    for (int i = 0; i < kTableSize; ++i) {
      data[i] = rand() % 100;
    }
  }

  virtual ~FilterBenchmark() {
    delete finder_;
  }

  virtual void Run(long long int number_of_repetitions) {
    int found = 0;
    if (number_of_repetitions < kTableSize) {
      found = finder_->Find(data, number_of_repetitions, result, selectivity_);
      data[found % kTableSize]++;
      data[(found + 1) % kTableSize]--;
      dummy_ += found;
    } else {
      for (int i = 0; i < number_of_repetitions / kTableSize; ++i) {
        found += finder_->Find(data, kTableSize, result, selectivity_);
        data[found % kTableSize]++;
        data[(found + 1) % kTableSize]--;
        dummy_ += found;
      }
    }
  }

 private:
  int selectivity_;
  Finder* finder_;
};

template <class F>
void RunBenchmarks(const char* description) {
  static const double kThousand = 1000;
  static const double kMega = 1024 * 1024;

  int selectivities[] = {0, 1, 5, 10, 25, 50, 75, 90, 95, 99, 100};
  int array_size = sizeof(selectivities) / sizeof(selectivities[0]);
  for (int i = 0; i < array_size; ++i) {
    Finder* finder = new F();
    Benchmark* benchmark = new FilterBenchmark(selectivities[i], finder);
    std::pair<int, long long int> result = benchmark->RunBenchmark();

    double bytes_processed = static_cast<double>(result.second) * sizeof(int);
    double seconds = static_cast<double>(result.first) / kThousand;
    printf("Throughput %s, selectivity %d: % 8.2fMB per second.\n",
           description, selectivities[i], (bytes_processed / kMega) / seconds);
    delete benchmark;
  }

}

int main() {
  RunBenchmarks<SmartFinder>("Smart finder");
  RunBenchmarks<NaiveFinder>("Naive finder");
  return 0;
}
