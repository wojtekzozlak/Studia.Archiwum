// Copyright 2012 Google Inc. All Rights Reserved.
// Author: onufry@google.com (Onufry Wojtaszczyk)
//
// A general benchmarking framework.
//
// Usage:
// Implement a subclass of Benchmark that implements the Run method. See
// comments on Run for details. After you implement this, you can run
// RunBenchmark to get results.

#ifndef GENERIC_BENCHMARK_H_HEADER_GUARD_
#define GENERIC_BENCHMARK_H_HEADER_GUARD_

#include <utility>

class Benchmark {
 public:
  Benchmark() : dummy_() {}
  virtual ~Benchmark() {}

  // Any setup that needs to happen before each call to Run. All the generic
  // setup that happens before all testing (like memory allocation) should
  // happen in the constructor.
  virtual void Setup() {}

  // The stuff we want to benchmark. Note that all the repetitions should be
  // identical, and their behaviour should not depend on the number of
  // repetitions, the typical implementation is simply a "for" loop. The reason
  // we provide this as an argument (instead of calling Run
  // |number_of_repetitions| times) is that we want to avoid paying the cost of
  // the virtual function call with each repetition.
  //
  // Take care to append the results of each repetition somewhere (a dummy
  // variable of the class seems a natural choice) to prevent the compiler from
  // optimizing all your calculations out. We provide a dummy variable for your
  // convenience.
  virtual void Run(long long int number_of_repetitions) = 0;

  // Runs the benchmark. Attempts to tweak the number of runs so that the
  // benchmark runs roughly for |expected_runtime_ms| milliseconds. Returns a
  // pair: the first element is the time of the benchmark (in milliseconds), and
  // the second is the number of repetitions that fit into this time. Usually
  // you are interested in first / second (which is the average time per cycle).
  std::pair<int, long long int> RunBenchmark(int expected_runtime_ms = 1000);

 protected:
  volatile int dummy_;
};
#endif  // GENERIC_BENCHMARK_H_HEADER_GUARD_
