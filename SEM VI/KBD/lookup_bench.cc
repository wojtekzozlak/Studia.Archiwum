#include <cstdio>
#include <sys/time.h>
#include <utility>
#include <cstdlib>
#include <algorithm>
#include <time.h>

#include "./generic_benchmark.h"

class LookupBenchmark : public Benchmark {
	public:
    LookupBenchmark(int table_size)
			: N(table_size), input_(new int[table_size]),
				data_(new int[table_size]), output_(new int[table_size]) {}

		virtual ~LookupBenchmark() {
			delete[] input_;
			delete[] output_;
			delete[] data_;
		}

    virtual void Setup() {
			// setting random data
			srand(time(0));
			for (int i = 0; i < N; ++i) {
				data_[i] = rand();
				input_[i] = i;
			}
			// shuffle input table
			std::random_shuffle(input_, input_ + N);
  	}

		virtual void Run(long long int number_of_repetitions) {
			for (long long int run = 0; run < number_of_repetitions; ++run) {
				for (int i = 0; i < N; ++i) {
					output_[i] = data_[input_[i]];
				}
				dummy_ = output_[0] * 10 + N;
			}
		}

	private:
	  int N;
		int *input_, *output_, *data_;
};

int main(void){
  static const double kMillion = 1000000;
  static const double kThousand = 1000;

  for (int N = 1 << 10; N < (1 << 20); N *= 2) {
    Benchmark* benchmark(new LookupBenchmark(N));
    std::pair<int, long long int> result = benchmark->RunBenchmark();

    double added_fields =
        static_cast<double>(result.second) * static_cast<double>(N) / kMillion;
    printf("Throughput for % 8dB: % 8.2fM per second.\n",
           N * sizeof(int), added_fields / (result.first / kThousand));
    delete benchmark;
  }

	return 0;
}
