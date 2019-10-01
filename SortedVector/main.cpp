#include "sorted_vector.hpp"
#include "typelist_utils.hpp"

#include <iostream>
#include <random>
#include <iterator>

struct Vector {
	Vector(int x, int y, int z) : x{x}, y{y}, z{z} {}

	int x = 0;
	int y = 0;
	int z = 0;
};

std::ostream& operator<<(std::ostream& os, const Vector& vec)
{
	os << "{ x = " << vec.x << "; y = " << vec.y << "; z = " << vec.z << "; }";
	return os;
}

struct CompareByX {
	bool operator()(const Vector& left, const Vector& right) const { return left.x < right.x; }
	bool operator()(const Vector& left, int x) const { return left.x < x; }
};

struct CompareByY {
	bool operator()(const Vector& left, const Vector& right) const { return left.y < right.y; }
	bool operator()(const Vector& left, int y) const { return left.y < y; }
};

struct CompareByZ {
	bool operator()(const Vector& left, const Vector& right) const { return left.z < right.z; }
	bool operator()(const Vector& left, int z) const { return left.z < z; }
};

int main()
{
	SortedCollection<Vector, CompareByX, CompareByY, CompareByZ> sortedVec;
	sortedVec.emplace(3, 1, 1);
	sortedVec.emplace(2, 2, 10);
	sortedVec.emplace(1, 3, 0);

	const auto foundIt = sortedVec.find<CompareByX>(3);
	if (foundIt != sortedVec.end<CompareByX>()) {
		std::cout << "Found Vector: " << *foundIt << '\n' << std::endl;
	}

	std::cout << "Sorted by X:" << std::endl;
	std::copy(sortedVec.begin<CompareByX>(), sortedVec.end<CompareByX>(), std::ostream_iterator<Vector>(std::cout, ",\n"));
	std::cout << std::endl;

	std::cout << "Sorted by Y:" << std::endl;
	std::copy(sortedVec.begin<CompareByY>(), sortedVec.end<CompareByY>(), std::ostream_iterator<Vector>(std::cout, ",\n"));
	std::cout << std::endl;

	std::cout << "Sorted by Z:" << std::endl;
	std::copy(sortedVec.begin<CompareByZ>(), sortedVec.end<CompareByZ>(), std::ostream_iterator<Vector>(std::cout, ",\n"));
	std::cout << std::endl;

	system("pause");
	return 0;
}