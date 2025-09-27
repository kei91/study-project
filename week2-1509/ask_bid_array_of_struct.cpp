#include <array>
#include <immintrin.h>
#include <benchmark/benchmark.h>

// A vector of structures representing an L2 order book snapshot :
// Example: askPrices[0] = 100, askVolumes[0] = 1 - someone is trying to sell 1 unit at 100 USD.
// Compute the total ask and bid volumes in the order book in USD.
// Maximize vectorization and use intrinsics

constexpr int N = 30;

struct Orderbook {
	double askPrice;
	double askVolume;
	double bidPrice;
	double bidVolume;

	Orderbook() {
		askPrice = 100;
		askVolume = 1;
		bidPrice = 100;
		bidVolume = 1;
	}
};

std::pair<double, double> arrayOfStruct(std::array<Orderbook, N>& orderBook)
{
	double askTotal = 0.0;
	double bidTotal = 0.0;
	for (int i = 0; i < N; ++i) {
		askTotal += orderBook[i].askPrice * orderBook[i].askVolume;
		bidTotal += orderBook[i].bidPrice * orderBook[i].bidVolume;
	}
	return { askTotal , bidTotal };
}

std::pair<double, double> arrayOfStructVec(std::array<Orderbook, N>& orderBook)
{
	double askTotal = 0.0;
	double bidTotal = 0.0;
	int i = 0;
	for (; i + 2 < N; i += 2) {
		askTotal += orderBook[i].askPrice * orderBook[i].askVolume;
		bidTotal += orderBook[i].bidPrice * orderBook[i].bidVolume;

		askTotal += orderBook[i + 1].askPrice * orderBook[i + 1].askVolume;
		bidTotal += orderBook[i + 1].bidPrice * orderBook[i + 1].bidVolume;
	}

	for (; i < N; ++i) {
		askTotal += orderBook[i].askPrice * orderBook[i].askVolume;
		bidTotal += orderBook[i].bidPrice * orderBook[i].bidVolume;
	}
	return { askTotal , bidTotal };
}

static void BM_ArrayOfStruct(benchmark::State& state) {
	std::array<Orderbook, N> orderBook;
	for (auto _ : state) {
		auto data = arrayOfStruct(orderBook);
		benchmark::DoNotOptimize(data);
		benchmark::ClobberMemory();
	}
}

static void BM_ArrayOfStructVec(benchmark::State& state) {
	std::array<Orderbook, N> orderBook;
	for (auto _ : state) {
		auto data = arrayOfStructVec(orderBook);
		benchmark::DoNotOptimize(data);
		benchmark::ClobberMemory();
	}
}

BENCHMARK(BM_ArrayOfStruct);
BENCHMARK(BM_ArrayOfStruct);
BENCHMARK(BM_ArrayOfStructVec);

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
BM_ArrayOfStruct          37.7 ns         35.6 ns     22400000
BM_ArrayOfStruct          29.7 ns         29.3 ns     22400000
BM_ArrayOfStructVec       27.2 ns         27.3 ns     26352941
*/
