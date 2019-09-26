#pragma once
#ifndef SORTED_VECTOR_HPP
#define SORTED_VECTOR_HPP

#include <vector>
#include <algorithm>

template<class T, class Comparator = std::less<T>, class Allocator = std::allocator<T>>
struct sorted_vector {
	using inner_container_type = std::vector<T, Allocator>;

	using value_type = typename inner_container_type::value_type;
	using allocator_type = typename inner_container_type::allocator_type;
	using size_type = typename inner_container_type::size_type;
	using difference_type = typename inner_container_type::difference_type;
	using reference = typename inner_container_type::reference;
	using const_reference = typename inner_container_type::const_reference;
	using pointer = typename inner_container_type::pointer;
	using const_pointer = typename inner_container_type::const_pointer;
	using iterator = typename inner_container_type::iterator;
	using const_iterator = typename inner_container_type::const_iterator;
	using reverse_iterator = typename inner_container_type::reverse_iterator;
	using const_reverse_iterator = typename inner_container_type::const_reverse_iterator;

	auto insert(const T& val)
	{
		return cont_.insert(std::lower_bound(std::cbegin(cont_), std::cend(cont_), val, Comparator()), val);
	}

	auto erase(const T& val)
	{
		auto it = std::lower_bound(std::cbegin(cont_), std::cend(cont_), val, Comparator());
		if (*it == val) {
			it = cont_.erase(it, val);
		}
		else {
			it = std::cend(cont_);
		}

		return it;
	}

	void eraseAll(const T& val)
	{
		auto itPair = std::equal_range(std::cbegin(cont_), std::cend(cont_), val, Comparator());
		cont_.erase(itPair.first, itPair.second);
	}

	template<class It>
	void assign(It first, It last) 
	{ 
		cont_.assign(first, last);
		std::sort(std::begin(cont_), std::end(cont_)); 
	}

	template<class Cont>
	void assign(const Cont& other) { assign(std::cbegin(other), std::cend(other)); }

	void clear() { cont_.clear(); }
	bool empty() const { return cont_.empty(); }

	size_type size() const { return cont_.size(); }
	size_type capacity() const { return cont_.capacity(); }

	void reserve(size_type sz) { cont_.reserve(sz); }
	void resize(size_type sz) { cont_.resize(sz); }
	void resize(size_type sz, const T& val) { cont_.resize(sz, val); }
	void shrink_to_fit() { cont_.shrink_to_fit(); }

	const T& at(size_type index) const { return cont_.at(index); }

	auto cbegin() const { return cont_.cbegin(); }
	auto cend() const { return cont_.cend(); }

	auto begin() const { return cbegin(); }
	auto end() const { return cend(); }

	auto crbegin() const { return cont_.crbegin(); }
	auto crend() const { return cont_.crend(); }

	auto rbegin() const { return crbegin(); }
	auto rend() const { return crend(); }

	void swap(sorted_vector& other) { cont_.swap(other.cont_); }

	friend bool operator==(sorted_vector& left, sorted_vector& right) { return left.cont_ == right.cont_; }
	friend bool operator!=(sorted_vector& left, sorted_vector& right) { return left.cont_ != right.cont_; }

	friend bool operator<(sorted_vector& left, sorted_vector& right) { return left.cont_ < right.cont_; }
	friend bool operator>=(sorted_vector& left, sorted_vector& right) { return left.cont_ >= right.cont_; }

	friend bool operator>(sorted_vector& left, sorted_vector& right) { return left.cont_ > right.cont_; }
	friend bool operator<=(sorted_vector& left, sorted_vector& right) { return left.cont_ <= right.cont_; }

private:
	inner_container_type cont_;
};

namespace std {
	template<class T, class Comparator, class Allocator>
	void swap(sorted_vector<T, Comparator, Allocator>& left, sorted_vector<T, Comparator, Allocator>& right) { left.swap(right); }
}

#endif // !SORTED_VECTOR_HPP