#include <iostream>
#include <thread>
#include <syncstream>
#include <vector>

//SPSC Lock-free bounded queue

constexpr int N = 8;

int main()
{
	std::vector<std::atomic<int>> data(N);
	std::fill(data.begin(), data.end(), -1);

	int write = 0;
	int read = 0;

	auto push = [&](int value) {
		while (data[write].load(std::memory_order_acquire) != -1) {
			std::this_thread::yield();
		}

		data[write].store(value, std::memory_order_release);

		write = (write + 1) & (N - 1);
	};

	auto pop = [&]() {
		while (data[read].load(std::memory_order_acquire) == -1) {
			std::this_thread::yield();
		}

		data[read].store(-1, std::memory_order_release);

		read = (read + 1) & (N - 1);
	};

	std::thread thread_read(pop);
	std::thread thread_write(push, 50);
	thread_write.join();
	thread_read.join();
}