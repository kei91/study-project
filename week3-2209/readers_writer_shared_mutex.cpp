#include <iostream>
#include <atomic>
#include <thread>
#include <syncstream>
#include <vector>

// multiple readers one writer with https://en.cppreference.com/w/cpp/thread/shared_mutex.html
int main()
{
	long long sum = 0;
	std::shared_mutex mutex;

	auto mutex_shared_read = [&](const int id) {

		mutex.lock_shared();

		std::osyncstream(std::cout)
			<< "sum: " << sum << '\n';

		mutex.unlock_shared();
	};

	auto mutex_unique_write = [&]() {
		for (int i = 0; i < 10000; ++i) {
			mutex.lock();
			++sum;
			mutex.unlock();
		}
	};

	std::vector<std::thread> threads_read;
	std::thread threads_write(mutex_unique_write);

	for (int i = 0; i < 100; ++i) {
		threads_read.emplace_back(mutex_shared_read, i);
	}

	threads_write.join();

	for (int i = 0; i < 100; ++i) {
		threads_read[i].join();
	}

	std::cout << sum << std::endl;
}
