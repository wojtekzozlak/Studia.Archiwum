// Copyright 2012 Google Inc. All Rights Reserved.
// Author: onufry@google.com (Onufry Wojtaszczyk)
//
// Benchmarks the costs of calling a virtual function, an non-inlined function,
// an inline function and a hard-coded operation.
// We will use addition of a constant to the argument as the internals of the
// function benchmarked in all the cases.
//
// Run with -O0, otheriwse the compiler notices that it can optimize the
// virtualization in Adder away.

#include <cstdio>
#include <utility>

#include "./generic_benchmark.h"

class Adder {
 public:
  __attribute__((noinline)) virtual void add(int& argument) {
    argument += 10;
  }
};

class PureAdder {
 public:
  __attribute__((noinline)) void add(int& argument) {
    argument += 10;
  }
};

class VirtualBenchmark : public Benchmark {
 public:
  VirtualBenchmark() { adder = new Adder(); }

  virtual ~VirtualBenchmark() { delete adder; }

  virtual void Run(long long int number_of_repetitions) {
    int dummy;
    for (long long int run = 0; run < number_of_repetitions; ++run) {
      adder->add(dummy);
    }
    dummy_ = dummy;
  }

 private:
  Adder* adder;
};

class PureMethodBenchmark : public Benchmark {
 public:
  PureMethodBenchmark() { adder = new PureAdder(); }

  virtual ~PureMethodBenchmark() { delete adder; }

  virtual void Run(long long int number_of_repetitions) {
    int dummy = 0;
    for (long long int run = 0; run < number_of_repetitions; ++run) {
      adder->add(dummy);
    }
    dummy_ = dummy;
  }

 private:
  PureAdder* adder;
};

__attribute__((noinline)) void function_add(int& argument) {
  argument += 10;
}

class FunctionBenchmark : public Benchmark {
 public:
  virtual void Run(long long int number_of_repetitions) {
    for (long long int run = 0; run < number_of_repetitions; ++run) {
      function_add(dummy);
    }
    dummy_ = dummy;
  }
  int dummy;
};

void inline inline_add(int& argument) {
  argument += 10;
}

class InlinedBenchmark : public Benchmark {
 public:
  virtual void Run(long long int number_of_repetitions) {
    for (long long int run = 0; run < number_of_repetitions; ++run) {
      inline_add(dummy);
    }
    dummy_ = dummy;
  }
  int dummy;
};

class HardcodedBenchmark : public Benchmark {
 public:
  virtual void Run(long long int number_of_repetitions) {
    for (long long int run = 0; run < number_of_repetitions; ++run) {
      dummy += 10;
    }
    dummy_ = dummy;
  }
  int dummy;
};

// Runs |benchmark| and prints the result using |description| (a description of
// the benchmark).
// Takes ownership of |benchmark|, but not of |description|.
void DoBenchmark(const char* description, Benchmark* benchmark) {
  static const double kThousand = 1000;
  static const double kMillion = 1000000;

  std::pair<int, int> result = benchmark->RunBenchmark();

  double millions_of_fields = static_cast<double>(result.second) / kMillion;
  printf("Throughput: % 8.2fM functions performed per second for %s.\n",
         millions_of_fields * kThousand / result.first, description);
  delete benchmark;
}

int main() {
  DoBenchmark("Hardcoded addition", new HardcodedBenchmark());
  DoBenchmark("Inlined addition", new InlinedBenchmark());
  DoBenchmark("Function addition", new FunctionBenchmark());
  DoBenchmark("Pure method addition", new PureMethodBenchmark());
  DoBenchmark("Virtual addition", new VirtualBenchmark());
}
