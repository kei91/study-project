#include <iostream>
#include <atomic>
#include <thread>
#include <vector>

int main()
{
	long long sum = 0;
	std::atomic<int> shared_counter{ 0 };
	std::atomic<bool> unique_lock{ false };
	std::atomic<bool> unique_waiting{ false };

	// https://en.cppreference.com/w/cpp/thread/shared_mutex.html with lock-free atomic
	auto mutex_shared_read = [&](const int id) {

		while (unique_lock.load(std::memory_order_acquire) || unique_waiting.load(std::memory_order_acquire)) {
			std::this_thread::yield();
		}

		shared_counter.fetch_add(1, std::memory_order_acquire);

		while (unique_lock.load(std::memory_order_acquire) || unique_waiting.load(std::memory_order_acquire)) {
			std::this_thread::yield();
		}

		std::cout << "sum: " << sum << '\n';

		shared_counter.fetch_sub(1, std::memory_order_release);
	};

	auto mutex_unique_write = [&](const int id) {

		while (shared_counter.load(std::memory_order_acquire) > 0) {
			std::this_thread::yield();
		}

		while (unique_waiting.exchange(true, std::memory_order_acquire)) {
			std::this_thread::yield();
		}

		while (unique_lock.exchange(true, std::memory_order_acquire)) {
			std::this_thread::yield();
		}

		for (int i = 0; i < 10000; ++i) {
			++sum;
		}

		unique_waiting.exchange(false, std::memory_order_release);
		unique_lock.exchange(false, std::memory_order_release);
	};

	std::vector<std::thread> threads_read;
	std::vector<std::thread> threads_write;

	for (int i = 0; i < 100; ++i) {
		threads_read.emplace_back(mutex_shared_read, i);
		threads_write.emplace_back(mutex_unique_write, i);
	}

	for (int i = 0; i < 100; ++i) {
		threads_read[i].join();
		threads_write[i].join();
	}

	std::cout << sum << std::endl;
}
