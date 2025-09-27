#include <array>
#include <immintrin.h>
#include <benchmark/benchmark.h>

// A vector of structures representing an L2 order book snapshot :
// Example: askPrices[0] = 100, askVolumes[0] = 1 - someone is trying to sell 1 unit at 100 USD.
// Compute the total ask and bid volumes in the order book in USD.
// Maximize vectorization and use intrinsics

constexpr int N = 30;

struct Orderbooks {
	std::array<double, N> askPrices;
	std::array<double, N> askVolumes;
	std::array<double, N> bidPrices;
	std::array<double, N> bidVolumes;

	Orderbooks() {
		for (int i = 0; i < N; ++i) {
			askPrices[i] = 100;
			askVolumes[i] = 1;
			bidPrices[i] = 100;
			bidVolumes[i] = 1;
		}
	}
};

std::pair<double, double> structOfArray(Orderbooks& orderBooks)
{
	double askTotal = 0.0;
	double bidTotal = 0.0;
	for (int i = 0; i < N; ++i) {
		askTotal += orderBooks.askPrices[i] * orderBooks.askVolumes[i];
		bidTotal += orderBooks.bidPrices[i] * orderBooks.bidVolumes[i];
	}

	return { askTotal , bidTotal };
}

// horizontal sum of 4 doubles
static inline double hsum256(__m256d v) {
	__m128d lo = _mm256_castpd256_pd128(v); // get first 128 bit
	__m128d hi = _mm256_extractf128_pd(v, 1); // get last 128 bit
	__m128d sum2 = _mm_add_pd(lo, hi);// get sum [a0+a2, a1+a3]
	__m128d shuf = _mm_shuffle_pd(sum2, sum2, 0x1); // shuf = [a1 + a3, a0 + a2], swap fisrt and last
	return _mm_cvtsd_f64(_mm_add_sd(sum2, shuf)); // (a0+a2)+(a1+a3)
}

std::pair<double, double> structOfArrayVec(Orderbooks& orderBooks)
{
	__m256d ask1 = _mm256_setzero_pd();
	__m256d bid1 = _mm256_setzero_pd();
	__m256d ask2 = _mm256_setzero_pd();
	__m256d bid2 = _mm256_setzero_pd();

	// cache line is 64 bytes
	// double size - 8 bytes -> one cache line holds 8 doubles
	// __m256d can store 4 doubles -> 256 bit == 32 byte
	// therefore we need two __m256d for one cache line
	int i = 0;
	for (; i + 8 < N; i += 8) {
		__m256d askPrice1 = _mm256_loadu_pd(orderBooks.askPrices.data() + i);
		__m256d askVolume1 = _mm256_loadu_pd(orderBooks.askVolumes.data() + i);
		ask1 = _mm256_fmadd_pd(askPrice1, askVolume1, ask1);

		__m256d askPrice2 = _mm256_loadu_pd(orderBooks.askPrices.data() + i + 4);
		__m256d askVolume2 = _mm256_loadu_pd(orderBooks.askVolumes.data() + i + 4);
		ask2 = _mm256_fmadd_pd(askPrice2, askVolume2, ask2);

		__m256d bidPrice1 = _mm256_loadu_pd(orderBooks.bidPrices.data() + i);
		__m256d bidVolume1 = _mm256_loadu_pd(orderBooks.bidVolumes.data() + i);
		bid1 = _mm256_fmadd_pd(bidPrice1, bidVolume1, bid1);

		__m256d bidPrice2 = _mm256_loadu_pd(orderBooks.bidPrices.data() + i + 4);
		__m256d bidVolume2 = _mm256_loadu_pd(orderBooks.bidVolumes.data() + i + 4);
		bid2 = _mm256_fmadd_pd(bidPrice2, bidVolume2, bid2);
	}

	double askRes = hsum256(ask1);
	askRes += hsum256(ask2);
	double bidRes = hsum256(bid1);
	bidRes += hsum256(bid2);

	for (; i < N; ++i) {
		askRes += orderBooks.askPrices[i] * orderBooks.askVolumes[i];
		bidRes += orderBooks.bidPrices[i] * orderBooks.bidVolumes[i];
	}

	return { askRes , bidRes };
}

static void BM_structOfArray(benchmark::State& state) {
	Orderbooks orderBooks;
	for (auto _ : state) {
		auto data = structOfArray(orderBooks);
		benchmark::DoNotOptimize(data);
		benchmark::ClobberMemory();
	}
}

static void BM_structOfArrayVec(benchmark::State& state) {
	Orderbooks orderBooks;
	for (auto _ : state) {
		auto data = structOfArrayVec(orderBooks);
		benchmark::DoNotOptimize(data);
		benchmark::ClobberMemory();
	}
}

BENCHMARK(BM_structOfArray);
BENCHMARK(BM_structOfArray);
BENCHMARK(BM_structOfArrayVec);

// Run the benchmark
BENCHMARK_MAIN();

/*
Run on (8 X 2304 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x4)
  L1 Instruction 32 KiB (x4)
  L2 Unified 256 KiB (x4)
  L3 Unified 8192 KiB (x1)
--------------------------------------------------------------
Benchmark                    Time             CPU   Iterations
--------------------------------------------------------------
BM_structOfArray          44.3 ns         37.0 ns     22400000
BM_structOfArray          30.8 ns         31.3 ns     19478261
BM_structOfArrayVec       14.9 ns         15.0 ns     44800000
*/
