#include <iostream>
#include <atomic>
#include <thread>
#include <vector>

int main()
{
	long long sum = 0;
	std::atomic<bool> flag{ false };

	// mutex with https://en.cppreference.com/w/cpp/atomic/atomic/compare_exchange.html
	auto mutex_exchange_sum = [&](const int id) {
		bool expected = false;
			while (!flag.compare_exchange_weak(expected, true, std::memory_order_acquire)) {
				expected = false;
				std::this_thread::yield();
			}

			for (int i = 0; i < 1000000; ++i) {
				++sum;
			}
			
			flag.store(false, std::memory_order_release);
		};

	std::vector<std::thread> threads;
	for (int i = 0; i < 200; ++i)
		threads.emplace_back(mutex_exchange_sum, i);

	for (int i = 0; i < 200; ++i)
		threads[i].join();

	std::cout << sum << std::endl;
}
