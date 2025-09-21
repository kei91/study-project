#include <array>
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
};

struct Orderbooks {
	std::array<double, N> askPrices;
	std::array<double, N> askVolumes;
	std::array<double, N> bidPrices;
	std::array<double, N> bidVolumes;
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

std::pair<double, double> structOfArrayVec(Orderbooks& orderBooks)
{
	double askTotal = 0.0;
	double bidTotal = 0.0;
	int i = 0;
	for (; i < N; i += 8) {
		askTotal += orderBooks.askPrices[i] * orderBooks.askVolumes[i];
		bidTotal += orderBooks.bidPrices[i] * orderBooks.bidVolumes[i];

		askTotal += orderBooks.askPrices[i + 1] * orderBooks.askVolumes[i + 1];
		bidTotal += orderBooks.bidPrices[i + 1] * orderBooks.bidVolumes[i + 1];

		askTotal += orderBooks.askPrices[i + 2] * orderBooks.askVolumes[i + 2];
		bidTotal += orderBooks.bidPrices[i + 2] * orderBooks.bidVolumes[i + 2];

		askTotal += orderBooks.askPrices[i + 3] * orderBooks.askVolumes[i + 3];
		bidTotal += orderBooks.bidPrices[i + 3] * orderBooks.bidVolumes[i + 3];

		askTotal += orderBooks.askPrices[i + 4] * orderBooks.askVolumes[i + 4];
		bidTotal += orderBooks.bidPrices[i + 4] * orderBooks.bidVolumes[i + 4];

		askTotal += orderBooks.askPrices[i + 5] * orderBooks.askVolumes[i + 5];
		bidTotal += orderBooks.bidPrices[i + 5] * orderBooks.bidVolumes[i + 5];

		askTotal += orderBooks.askPrices[i + 6] * orderBooks.askVolumes[i + 6];
		bidTotal += orderBooks.bidPrices[i + 6] * orderBooks.bidVolumes[i + 6];

		askTotal += orderBooks.askPrices[i + 7] * orderBooks.askVolumes[i + 7];
		bidTotal += orderBooks.bidPrices[i + 7] * orderBooks.bidVolumes[i + 7];
	}


	for (; i < N; ++i) {
		askTotal += orderBooks.askPrices[i] * orderBooks.askVolumes[i];
		bidTotal += orderBooks.bidPrices[i] * orderBooks.bidVolumes[i];
	}

	return { askTotal , bidTotal };
}

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
	for (; i < N; i += 2) {
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

static void BM_structOfArray(benchmark::State& state) {

	Orderbooks orderBooks;
	for (int i = 0; i < N; ++i) {
		orderBooks.askPrices[i] = rand() % 100;
		orderBooks.askVolumes[i] = rand() % 100;
		orderBooks.bidPrices[i] = rand() % 100;
		orderBooks.bidVolumes[i] = rand() % 100;
	}

	for (auto _ : state) {
		auto data = structOfArray(orderBooks);
		benchmark::DoNotOptimize(data);
		benchmark::ClobberMemory();
	}
}

static void BM_structOfArrayVec(benchmark::State& state) {

	Orderbooks orderBooks;
	for (int i = 0; i < N; ++i) {
		orderBooks.askPrices[i] = rand() % 100;
		orderBooks.askVolumes[i] = rand() % 100;
		orderBooks.bidPrices[i] = rand() % 100;
		orderBooks.bidVolumes[i] = rand() % 100;
	}

	for (auto _ : state) {
		auto data = structOfArrayVec(orderBooks);
		benchmark::DoNotOptimize(data);
		benchmark::ClobberMemory();
	}
}

static void BM_ArrayOfStruct(benchmark::State& state) {
	
	std::array<Orderbook, N> orderBook;
	for (int i = 0; i < N; ++i) {
		orderBook[i].askPrice = rand() % 100;
		orderBook[i].askVolume = rand() % 100;
		orderBook[i].bidPrice = rand() % 100;
		orderBook[i].bidVolume = rand() % 100;
	}

	for (auto _ : state) {
		auto data = arrayOfStruct(orderBook);
		benchmark::DoNotOptimize(data);
		benchmark::ClobberMemory();
	}
}

static void BM_ArrayOfStructVec(benchmark::State& state) {

	std::array<Orderbook, N> orderBook;
	for (int i = 0; i < N; ++i) {
		orderBook[i].askPrice = rand() % 100;
		orderBook[i].askVolume = rand() % 100;
		orderBook[i].bidPrice = rand() % 100;
		orderBook[i].bidVolume = rand() % 100;
	}

	for (auto _ : state) {
		auto data = arrayOfStructVec(orderBook);
		benchmark::DoNotOptimize(data);
		benchmark::ClobberMemory();
	}
}

BENCHMARK(BM_structOfArray);
BENCHMARK(BM_structOfArray);
BENCHMARK(BM_structOfArrayVec);
BENCHMARK(BM_ArrayOfStruct);
BENCHMARK(BM_ArrayOfStructVec);

// Run the benchmark
BENCHMARK_MAIN();

/*
Run on(8 X 2304 MHz CPU s)
CPU Caches :
L1 Data 32 KiB(x4)
L1 Instruction 32 KiB(x4)
L2 Unified 256 KiB(x4)
L3 Unified 8192 KiB(x1)
--------------------------------------------------------------
Benchmark                    Time             CPU   Iterations
--------------------------------------------------------------
BM_structOfArray          37.7 ns         36.1 ns     20363636
BM_structOfArray          30.1 ns         30.0 ns     22400000
BM_structOfArrayVec       31.5 ns         31.8 ns     23578947
BM_ArrayOfStruct          29.5 ns         29.3 ns     22400000
BM_ArrayOfStructVec       26.8 ns         26.1 ns     26352941
*/
