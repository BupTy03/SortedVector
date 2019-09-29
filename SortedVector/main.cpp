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

struct CompareByX {
	bool operator()(const Vector& left, const Vector& right) const { return left.x < right.x; }
};

struct CompareByY {
	bool operator()(const Vector& left, const Vector& right) const { return left.y < right.y; }
};

struct CompareByZ {
	bool operator()(const Vector& left, const Vector& right) const { return left.z < right.z; }
};

int main()
{
	sorted_vector<Vector, std::allocator<Vector>, CompareByX, CompareByY, CompareByZ> sortedVec;
	sortedVec.emplace(3, 2, 1);
	sortedVec.emplace(1, 2, 3);
	sortedVec.emplace(0, 0, 0);

	system("pause");
	return 0;
}