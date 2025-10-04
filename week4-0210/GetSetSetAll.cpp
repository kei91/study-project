#include <iostream>
#include <vector>

//GetSetSetAll
constexpr int N = 8;

int main()
{
	int epoch = 0;
	std::vector<std::pair<int, int>> data(N);
	std::fill(data.begin(), data.end(), std::pair<int, int>(epoch,0));

	std::pair<int, int> setAllData(epoch, 0);

	auto get = [&](int index) -> int {
		if (setAllData.first > data[index].first)
			return setAllData.second;

		return data[index].second;
	};

	auto set = [&](int index, int value) {
		++epoch;
		data[index].first = epoch;
		data[index].second = value;
	};

	auto setAll = [&](int value) {
		++epoch;
		setAllData.first = epoch;
		setAllData.second = value;
	};

	std::cout << get(0) << std::endl;

	set(0, 1);
	std::cout << get(0) << std::endl;

	setAll(2);
	std::cout << get(0) << std::endl;

	set(0, 3);
	std::cout << get(0) << std::endl;
}