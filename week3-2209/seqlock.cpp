#include <iostream>
#include <thread>
#include <syncstream>
#include <vector>

// seqlock
int main()
{
	int epoch = 0;
	long long sum = 0;

	auto reader = [&](const int id) {

		int before = epoch;
		long long tempSum = sum;
		int after = epoch;

		while (after % 2 == 1 || before % 2 == 1 || before != after) {
			before = epoch;
			tempSum = sum;
			after = epoch;
		}

		std::osyncstream(std::cout) << "sum: " << tempSum << '\n';
	};

	auto writer = [&]() {
		for (int i = 0; i < 10000; ++i) {
			++epoch;
			++sum;
			++epoch;
		}
	};

	std::vector<std::thread> threads_read;
	std::thread threads_write(writer);

	for (int i = 0; i < 100; ++i) {
		threads_read.emplace_back(reader, i);
	}

	for (int i = 0; i < 100; ++i) {
		threads_read[i].join();
	}

	threads_write.join();

	std::cout << sum << std::endl;
}