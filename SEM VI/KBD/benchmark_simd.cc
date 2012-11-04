// Copyright 2012 Google Inc. All Rights Reserved.
// Author: ptab@google.com (Piotr Tabor)

#include <cstdio>
#include <string>
#include <stdint.h>
#include <sys/time.h>
#include <utility>

#include "xmmintrin.h"

#include "./generic_benchmark.h"

struct SumNoSIMD {
  void operator()(int32_t* a1, int32_t* a2, /*int*/ size_t size, int32_t* res) const { 
    for (int i = 0; i < size; ++i) {
      res[i] = a1[i] + a2[i];
    }
  }

  static const std::string describe()  { return "SumNoSIMD"; }
};

struct SumSIMD {
  void operator()(int32_t* a1, int32_t* a2, int size, int32_t* res) const { 
    SumNoSIMD no_simd;
    if  ((reinterpret_cast<intptr_t>(a1) % 16 != reinterpret_cast<intptr_t>(res) % 16)
        || (reinterpret_cast<intptr_t>(a1) % 16 != reinterpret_cast<intptr_t>(res) % 16) 
        || (reinterpret_cast<intptr_t>(a1) % sizeof(*a1) != 0)) {
      no_simd(a1, a2, size, res);
      return; 
    } 
    
    const int skip_items = ((reinterpret_cast<intptr_t>(a1) - 16) % 16) / sizeof(*a1);
    no_simd(a1, a2, skip_items, res);
    const int step = 16 / sizeof(*a1);
    int i;
    for (i = skip_items; i < size - step; i += step) {
       __m128i xmm1 = _mm_load_si128(reinterpret_cast<const __m128i*>(a1 + i));
       __m128i xmm2 = _mm_load_si128(reinterpret_cast<const __m128i*>(a2 + i));
       __m128i xmmr = _mm_add_epi32(xmm1, xmm2);     //  SUM  4 32-bit words
       _mm_store_si128(reinterpret_cast<__m128i*>(res + i), xmmr);
    }
    no_simd(a1 + i, a2 + i, size - i, res + i);
  }

  static const std::string describe()  { return "SumSIMD"; }
};


// Benchmark - how much time does it take to add N integers?
template <class Op>
class SimdBenchmark : public Benchmark {
 public:
  SimdBenchmark(int table_size)
      : N(table_size),
        table_1_(new int[table_size]),
        table_2_(new int[table_size]), 
        table_res_(new int[table_size]) {}

  virtual ~SimdBenchmark() { 
    delete[] table_1_; 
    delete[] table_2_;
    delete[] table_res_;
  }

  virtual void Setup() {
    // Set up some rather random values in the table.
    timeval current_time;
    gettimeofday(&current_time, NULL);
    table_1_[0] = current_time.tv_sec;
    table_2_[1] = current_time.tv_usec;
    for (int i = 1; i < N; ++i) {
      table_1_[i] = table_1_[i - 1] ^ ((N + 7) * i);
      table_2_[i] = table_2_[i - 1] ^ ((N + 13) * i);
    }
  }

  virtual void Run(long long int number_of_repetitions) {
    Op op;
    for (long long int run = 0; run < number_of_repetitions; ++run) {
      op(table_1_, table_2_, N, table_res_);
      table_1_[N & run]++; // Mutate the data.
      dummy_ += table_res_[N & run];
    }
  }

 private:
  int N;
  int32_t* table_1_;
  int32_t* table_2_;
  int32_t* table_res_;
};

template <class Op>
void BenchmarkOp() {
  static const double kMillion = 1000000;
  static const double kThousand = 1000;
  printf("=========================================================\n");
  printf("Benchmarking: %s\n", Op::describe().c_str());
  for (int N = 1 << 10; N < (1 << 20); N *= 2) {
    Benchmark* benchmark(new SimdBenchmark<Op>(N));
    std::pair<int, long long int> result = benchmark->RunBenchmark();

    double added_fields =
        static_cast<double>(result.second) * static_cast<double>(N) / kMillion;
    printf("Throughput for % 8d ints: % 8.2fM per second.\n",
           N, added_fields / (result.first / kThousand));
    delete benchmark;
  }
}

int main() {
  BenchmarkOp<SumSIMD>();
  BenchmarkOp<SumNoSIMD>();
}
