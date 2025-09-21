#include <iostream>
#include <atomic>
#include <thread>
#include <vector>

int main()
{
	long long sum = 0;
	std::atomic<bool> flag{ false };

	// mutex with https://en.cppreference.com/w/cpp/atomic/atomic/exchange.html
	auto mutex_exchange_sum = [&](const int id) {
			while (flag.exchange(true, std::memory_order_acquire)) {
				std::this_thread::yield();
			}

			for (int i = 0; i < 1000000; ++i) {
				++sum;
			}
			
			flag.exchange(false, std::memory_order_release);
		};

	std::vector<std::thread> threads;
	for (int i = 0; i < 200; ++i)
		threads.emplace_back(mutex_exchange_sum, i);

	for (int i = 0; i < 200; ++i)
		threads[i].join();

	std::cout << sum << std::endl;
}
