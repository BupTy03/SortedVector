#include "sorted_vector.hpp"

#include <iostream>
#include <random>
#include <iterator>

int main()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(1, 100);

	const auto rand_int_generator = [&dist, &gen] { return dist(gen); };

	std::vector<int> vec(10);
	std::generate(std::begin(vec), std::end(vec), rand_int_generator);

	sorted_vector<int> sorted_vec;
	for (int elem : vec) {
		sorted_vec.insert(elem);
	}

	std::ostream_iterator<int> ostream_it{ std::cout, " " };

	std::cout << "Usual array: ";
	std::copy(std::cbegin(vec), std::cend(vec), ostream_it);
	std::cout << std::endl;

	std::cout << "Sorted array: ";
	std::copy(std::cbegin(sorted_vec), std::cend(sorted_vec), ostream_it);
	std::cout << std::endl;

	system("pause");
	return 0;
}