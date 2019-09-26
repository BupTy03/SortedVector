#pragma once
#ifndef ASSOC_VECTOR_HPP
#define ASSOC_VECTOR_HPP

#include <utility>
#include <vector>
#include <algorithm>

template<class Comp>
struct CompareFirst {
	template<class First, class Second>
	bool operator()(const std::pair<First, Second>& left, const First& right) const {
		return Comp()(left.first, right);
	}
};

template<class ForwardIt, class T, class Compare = std::less<>>
ForwardIt binary_find(ForwardIt first, ForwardIt last, const T& value, Compare comp = {})
{
	first = std::lower_bound(first, last, value, comp);
	return first != last && !comp(value, *first) ? first : last;
}

template<
	class Key, 
	class T, 
	class Comparator = std::less<Key>, 
	class Allocator = std::allocator<std::pair<const Key, T>>
>
struct assoc_vector {
	using inner_container_type = std::vector<std::pair<const Key, T>, Allocator>;

	using value_type				= typename inner_container_type::value_type;
	using allocator_type			= typename inner_container_type::allocator_type;
	using size_type					= typename inner_container_type::size_type;
	using difference_type			= typename inner_container_type::difference_type;
	using reference					= typename inner_container_type::reference;
	using const_reference			= typename inner_container_type::const_reference;
	using pointer					= typename inner_container_type::pointer;
	using const_pointer				= typename inner_container_type::const_pointer;
	using iterator					= typename inner_container_type::iterator;
	using const_iterator			= typename inner_container_type::const_iterator;
	using reverse_iterator			= typename inner_container_type::reverse_iterator;
	using const_reverse_iterator	= typename inner_container_type::const_reverse_iterator;

	T& operator[](const Key& key)
	{
		const auto it = binary_find(std::begin(cont_), std::end(cont_), key, CompareFirst<Comparator>());
		if (it == std::end(cont_)) {
			throw std::out_of_range{ "key is out of range" };
		}
		return *it;
	}
	const T& operator[](const Key& key) const
	{
		const auto it = binary_find(std::cbegin(cont_), std::cend(cont_), key, CompareFirst<Comparator>());
		if (it == std::cend(cont_)) {
			throw std::out_of_range{ "key is out of range" };
		}
		return *it;
	}

	template<class P>
	std::pair<iterator, bool> insert(P&& val)
	{
		const auto first = std::cbegin(cont_);
		const auto last = std::cend(cont_);
		const auto it = std::lower_bound(first, last, val, Comparator());
		if (first == last || !comp(val, *first) {
			return std::make_pair(last, false);
		}
		return std::make_pair(cont_.insert(it, std::forward<P>(val)), true);
	}

	// DODELAT !!!
	/*std::pair<iterator, bool> insert(const_iterator hint, const value_type& value)
	{
		if (hint < std::cbegin(cont_) || hint > std::cend(cont_)) {
			throw std::out_of_range{"iterator is out of range"};
		}

		const bool valueLessHint = comp(value, *hint);

		if ((hint == std::cend(cont_) || valueLessHint) && comp(*(hint - 1), value)) {
			std::make_pair(cont_.insert(hint, value, true));
		}

		const Comparator comp;
		if (comp(hint->first, value.first) && (hint ==  !comp())) {
			std::make_pair(cont_.insert(hint, value, true));
		}
		else if()
	}*/

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

	auto begin() { return cont_.begin(); }
	auto end() { return cont_.end(); }

	auto cbegin() const { return cont_.cbegin(); }
	auto cend() const { return cont_.cend(); }

	auto begin() const { return cbegin(); }
	auto end() const { return cend(); }

	auto rbegin() { return cont_.rbegin(); }
	auto rend() { return cont_.rend(); }

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

#endif // !ASSOC_VECTOR_HPP