#pragma once
#ifndef ALGORITHMS_UTILS
#define ALGORITHMS_UTILS

#include <algorithm>
#include <functional>
#include <utility>
#include <set>


template<class Comp>
struct CompareFirstAdapter {
	template<class First, class Second>
	bool operator()(const std::pair<First, Second>& left, const std::pair<First, Second>& right) const {
		return comp_(left.first, right.first);
	}

	template<class First, class Second>
	bool operator()(const std::pair<First, Second>& left, const First& right) const {		
		return comp_(left.first, right);
	}

	template<class First, class Second>
	bool operator()(const First& left, const std::pair<First, Second>& right) const {
		return comp_(left, right.first);
	}

private:
	Comp comp_;
};

struct ComparePairByFirst {
	template<class TypeFirst, class TypeSecond>
	bool operator()(const TypeFirst& left, const std::pair<TypeFirst, TypeSecond>& right) const { return left < right.first; }

	template<class TypeFirst, class TypeSecond>
	bool operator()(const std::pair<TypeFirst, TypeSecond>& left, const TypeFirst& right) const { return left.first < right; }
};

struct ComparePairBySecond {
	template<class TypeFirst, class TypeSecond>
	bool operator()(const TypeSecond& left, const std::pair<TypeFirst, TypeSecond>& right) const { return left < right.second; }

	template<class TypeFirst, class TypeSecond>
	bool operator()(const std::pair<TypeFirst, TypeSecond>& left, const TypeSecond& right) const { return left.second < right; }
};

template<class ForwardIt, class T, class Compare = std::less<>>
ForwardIt binary_find(ForwardIt first, ForwardIt last, const T& value, Compare comp = {})
{
	first = std::lower_bound(first, last, value, comp);
	return first != last && !comp(value, *first) ? first : last;
}

template<class Container, class T, class Compare = std::less<>>
auto binary_find(Container& cont, const T& value, Compare comp = {}) { return binary_find(std::begin(cont), std::end(cont), value, comp); }

template<class Container, class T, class Compare = std::less<>>
auto binary_find(const Container& cont, const T& value, Compare comp = {}) { return binary_find(std::cbegin(cont), std::cend(cont), value, comp); }

template<class ForwardIt, class T, class Compare = std::less<>>
auto binary_find_range(ForwardIt first, ForwardIt last, const T& value, Compare comp = {})
{
	first = std::lower_bound(first, last, value, comp);
	if (first == last || comp(value, *first)) {
		return std::make_pair(last, last);
	}

	return std::make_pair(first, std::upper_bound(first, last, value, comp));
}

template<class Container, class T, class Compare = std::less<>>
auto binary_find_range(Container& cont, const T& value, Compare comp = {}) { return binary_find_range(std::begin(cont), std::end(cont), value, comp); }

template<class Container, class T, class Compare = std::less<>>
auto binary_find_range(const Container& cont, const T& value, Compare comp = {}) { return binary_find_range(std::cbegin(cont), std::cend(cont), value, comp); }

template<class T, class Pred>
void remove_if(std::set<T>& cont, Pred pred = {})
{
	for (auto it = std::cbegin(cont); it != std::cend(cont); ) {
		if (pred(*it)) {
			it = cont.erase(it);
		}
		else {
			++it;
		}
	}
}

#endif // !ALGORITHMS_UTILS
