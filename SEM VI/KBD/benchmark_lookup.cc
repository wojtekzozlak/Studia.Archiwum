// Copyright 2012 Google Inc. All Rights Reserved.
// Author: onufry@google.com (Onufry Wojtaszczyk)
//
// Benchmarks a lookup function. Most interesting under -O6

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <utility>

#include "./generic_benchmark.h"

// We benchmark a lookup function: iterating over a table of size |table_size|
// and running result_[pos] = table_[input_[pos]]; for each entry in the table.
// We are interested in how the timing changes with the size of the table.
class LookupBenchmark : public Benchmark {
 public:
  LookupBenchmark(int table_size)
      : table_size_(table_size),
        result_(new int[table_size]),
        table_(new int[table_size]),
        input_(new int[table_size]) {
    assert(result_ != NULL);
    assert(table_ != NULL);
    assert(input_ != NULL);
  }

  virtual ~LookupBenchmark() {
    delete[] result_;
    delete[] table_;
    delete[] input_;
  }

  // Fill the input_ table with a random permutation of [0, ..., table_size - 1]
  // and fill the table_ table with some random data.
  virtual void Setup() {
    for (int i = 0; i < table_size_; ++i) {
      table_[i] = rand();
      input_[i] = i;
    }
    for (int i = 0; i < table_size_; ++i) {
      // Pseudorandomly permute the numbers in input_.
      std::swap(input_[i], input_[i + (rand() % (table_size_ - i))]);
    }
  }

  virtual void Run(long long int number_of_repetitions) {
    for (long long int run = 0; run < number_of_repetitions; ++run) {
      int pos;
      for (pos = 0; pos < table_size_; ++pos) {
        result_[pos] = table_[input_[pos]];  // All the time goes here.
      }
      // Dummy line to make the computation look useful to the compiler.
      dummy_ += result_[result_[0] * pos % table_size_];
      // Another dummy line, to make the runs differ.
      table_[0] = run % table_size_;
    }
  }

 private:
  // The size of all the input tables.
  int table_size_;

  int* result_;
  int* table_;
  // Contains a pseudorandom permutation of integers from 0 to table_size_ - 1.
  int* input_;
};

int main(int argc, char* argv[]) {
  // Run the benchmark for various table sizes, between 1 << 5 and 1 << 24.
  for (int log_table_size = 5; log_table_size < 26; ++log_table_size) {
    int table_size = 1 << log_table_size;
    Benchmark* benchmark = new LookupBenchmark(table_size);
    std::pair<int, int> result = benchmark->RunBenchmark();

    double number_of_fields_processed =
        static_cast<double>(result.second) * static_cast<double>(table_size);
    double megabytes_processed =
        number_of_fields_processed * sizeof(int) / (1 << 20);

    char buffer[200];
    int input_bytes = static_cast<int>(table_size * sizeof(int));
    sprintf(buffer, "%6dB", input_bytes);
    if (input_bytes > 1024) {
      sprintf(buffer, "%5dKB", input_bytes / 1024);
    }
    if (input_bytes > 1024 * 1024) {
      sprintf(buffer, "%5dMB", input_bytes / (1024 * 1024));
    }
    printf("For %s of input: % 8.2fMB processed per second\n",
           buffer, megabytes_processed * 1000. / result.first);
    delete benchmark;
  }
}
