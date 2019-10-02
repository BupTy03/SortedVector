#include "sorted_vector.hpp"
#include "typelist_utils.hpp"

#include <iostream>
#include <random>
#include <iterator>

struct Vector {
	Vector(int x, int y, int z) : x{x}, y{y}, z{z} {}

	bool operator==(const Vector& other) const { return x == other.x && y == other.y && z == other.z; }
	bool operator!=(const Vector& other) const { return !(*this == other); }

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
	bool operator()(const Vector& left, int right) const { return left.x < right; }
	bool operator()(int left, const Vector& right) const { return left < right.x; }
};

struct CompareByY {
	bool operator()(const Vector& left, const Vector& right) const { return left.y < right.y; }
	bool operator()(const Vector& left, int right) const { return left.y < right; }
	bool operator()(int left, const Vector& right) const { return left < right.y; }
};

struct CompareByZ {
	bool operator()(const Vector& left, const Vector& right) const { return left.z < right.z; }
	bool operator()(const Vector& left, int right) const { return left.z < right; }
	bool operator()(int left, const Vector& right) const { return left < right.z; }
};

int main()
{
	SortedCollection<Vector, CompareByX, CompareByY, CompareByZ> sortedVec;
	sortedVec.reserve(5);

	sortedVec.emplace(3, 1, 1);
	sortedVec.emplace(0, 0, 0);
	sortedVec.emplace(2, 2, 10);
	sortedVec.emplace(1, 3, 0);
	sortedVec.emplace(0, 0, 0);
	sortedVec.emplace(3, 4, 5);
	sortedVec.emplace(3, 7, 1);
	sortedVec.emplace(0, 0, 0);
	sortedVec.emplace(1, 1, 1);


	const auto foundIt = sortedVec.find<CompareByX>(3);
	if (foundIt != sortedVec.end<CompareByX>()) {
		std::cout << "Found Vector: " << *foundIt << '\n' << std::endl;
	}

	const auto foundBounds = sortedVec.findAll<CompareByX>(3);
	std::cout << "Found Vectors: " << std::endl;
	std::copy(foundBounds.first, foundBounds.second, std::ostream_iterator<Vector>(std::cout, ",\n"));
	std::cout << std::endl;

	int arrX[] = { 1, 2, 3, 3, 3 };
	if (sortedVec.compare<CompareByX>(arrX) == 0) {
		std::cout << "sortedVec equal to arrX by X\n" << std::endl;
	}

	std::cout << "Sorted by X:" << std::endl;
	std::copy(sortedVec.begin<CompareByX>(), sortedVec.end<CompareByX>(), std::ostream_iterator<Vector>(std::cout, ",\n"));
	std::cout << std::endl;

	sortedVec.erase({1, 1, 1});

	std::cout << "Sorted by Y:" << std::endl;
	std::copy(sortedVec.begin<CompareByY>(), sortedVec.end<CompareByY>(), std::ostream_iterator<Vector>(std::cout, ",\n"));
	std::cout << std::endl;

	sortedVec.eraseAll({ 0, 0, 0 });

	std::cout << "Sorted by Z:" << std::endl;
	std::copy(sortedVec.begin<CompareByZ>(), sortedVec.end<CompareByZ>(), std::ostream_iterator<Vector>(std::cout, ",\n"));
	std::cout << std::endl;

	std::cout << "Sorted by X in reverse:" << std::endl;
	std::copy(sortedVec.rbegin<CompareByX>(), sortedVec.rend<CompareByX>(), std::ostream_iterator<Vector>(std::cout, ",\n"));
	std::cout << std::endl;

	system("pause");
	return 0;
}