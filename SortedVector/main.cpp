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
	sorted_vector<Vector, std::allocator<Vector>, CompareByX, CompareByY, CompareByZ> sortedVec;
	sortedVec.emplace(3, 2, 1);
	sortedVec.emplace(1, 2, 3);
	sortedVec.emplace(0, 0, 0);

	const auto vec1 = sortedVec.findByComparatorOrDefault<CompareByX>(3, Vector{ 0, 0, 0 });
	std::cout << "vec1: " << vec1 << std::endl;


	system("pause");
	return 0;
}