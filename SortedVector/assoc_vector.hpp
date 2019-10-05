#pragma once
#ifndef ASSOC_VECTOR_HPP
#define ASSOC_VECTOR_HPP

#include "algorithms_utils.hpp"

#include <utility>
#include <vector>
#include <algorithm>
#include <cassert>


template<
	class Key, 
	class T, 
	class Comparator = std::less<Key>, 
	class Allocator = std::allocator<std::pair<Key, T>>
>
struct assoc_vector {
	using inner_container_type = std::vector<std::pair<Key, T>, Allocator>;

	using value_type = typename inner_container_type::value_type;
	using allocator_type = typename inner_container_type::allocator_type;
	using size_type	= typename inner_container_type::size_type;
	using difference_type = typename inner_container_type::difference_type;
	using reference = typename inner_container_type::reference;
	using const_reference = typename inner_container_type::const_reference;
	using pointer = typename inner_container_type::pointer;
	using const_pointer = typename inner_container_type::const_pointer;
	using iterator = typename inner_container_type::iterator;
	using const_iterator = typename inner_container_type::const_iterator;
	using reverse_iterator = typename inner_container_type::reverse_iterator;
	using const_reverse_iterator = typename inner_container_type::const_reverse_iterator;

	T& at(const Key& key)
	{
		const auto it = binary_find(std::begin(elems_), std::end(elems_), key, CompareFirstAdapter<Comparator>());
		if (it == std::end(elems_)) {
			throw std::out_of_range{ "key is out of range" };
		}
		return *it;
	}
	const T& at(const Key& key) const
	{
		const auto it = binary_find(std::cbegin(elems_), std::cend(elems_), key, CompareFirstAdapter<Comparator>());
		if (it == std::cend(elems_)) {
			throw std::out_of_range{ "key is out of range" };
		}
		return *it;
	}
	T& operator[](const Key& key)
	{
		CompareFirstAdapter<Comparator> comp;
		auto it = std::lower_bound(std::begin(elems_), std::end(elems_), key, comp);
		if (it == std::end(elems_) || comp(key, *it)) {
			it = this->emplace_hint(it, key, T());
		}

		assert(it != std::end(elems_));
		return it->second;
	}
	const T& operator[](const Key& key) const { return at(key); }

	std::pair<iterator, bool> insert(const value_type& value)
	{
		const auto it = std::lower_bound(std::begin(elems_), std::end(elems_), value, CompareFirstAdapter<Comparator>());
		return std::make_pair(elems_.insert(it, value), true);
	}
	iterator insert(const_iterator hint, const value_type& value)
	{
		if (hint < std::cbegin(elems_) || hint > std::cend(elems_)) {
			throw std::out_of_range{"iterator is out of range"};
		}

		CompareFirstAdapter<Comparator> comp;
		const auto first = std::cbegin(elems_);
		const auto last = std::cend(elems_);

		if ((hint == last && (hint == first || !comp(value, *(std::prev(hint))))) ||	// if no elems or value isn't less than prev element of last
			(hint == first && comp(value, *hint)) ||									// if value is less than first
			(!comp(value, *(std::prev(hint))) && comp(value, *hint)))					// if value is less than hint and not less than prev
		{
			return elems_.insert(hint, value);
		}
		
		return this->insert(value).first;
	}
	template< class InputIt >
	void insert(InputIt first, InputIt last)
	{
		for (; first != last; ++first) {
			this->insert(*first);
		}
	}
	void insert(std::initializer_list<value_type> ilist) { this->insert(ilist.begin(), ilist.end()); }

	template<class... Args>
	std::pair<iterator, bool> emplace(Args&&... args)
	{
		value_type value(std::forward<Args>(args)...);
		const auto it = std::lower_bound(std::begin(elems_), std::end(elems_), value, CompareFirstAdapter<Comparator>());
		return std::make_pair(elems_.insert(it, std::move(value)), true);
	}
	template<class... Args>
	iterator emplace_hint(const_iterator hint, Args&&... args)
	{
		if (hint < std::cbegin(elems_) || hint > std::cend(elems_)) {
			throw std::out_of_range{ "iterator is out of range" };
		}

		CompareFirstAdapter<Comparator> comp;
		const auto first = std::cbegin(elems_);
		const auto last = std::cend(elems_);

		value_type value(std::forward<Args>(args)...);

		if ((hint == last && (hint == first || !comp(value, *(std::prev(hint))))) ||	// if no elems or value isn't less than prev element of last
			(hint == first && comp(value, *hint)) ||									// if value is less than first
			(!comp(value, *(std::prev(hint))) && comp(value, *hint)))					// if value is less than hint and not less than prev
		{
			return elems_.insert(hint, std::move(value));
		}

		return this->insert(value).first;
	}

	iterator erase(const value_type& value)
	{
		auto it = binary_find(elems_, value, CompareFirstAdapter<Comparator>());
		return (it != std::end(elems_)) ? elems_.erase(it) : it;
	}
	void eraseAll(const value_type& value)
	{
		const auto itPair = std::equal_range(std::cbegin(elems_), std::cend(elems_), value, CompareFirstAdapter<Comparator>());
		elems_.erase(itPair.first, itPair.second);
	}

	iterator find(const Key& key) { return binary_find(elems_, key, CompareFirstAdapter<Comparator>()); }
	const_iterator find(const Key& key) const { return binary_find(elems_, key, CompareFirstAdapter<Comparator>()); }

	std::pair<iterator, iterator> equal_range(const Key& key) { return std::equal_range(std::begin(elems_), std::end(elems_), key, CompareFirstAdapter<Comparator>()); }
	std::pair<const_iterator, const_iterator> equal_range(const Key& key) const { return std::equal_range(std::cbegin(elems_), std::cend(elems_), key, CompareFirstAdapter<Comparator>()); }

	iterator lower_bound(const Key& key) { return std::lower_bound(std::begin(elems_), std::end(elems_), key, CompareFirstAdapter<Comparator>()); }
	const_iterator lower_bound(const Key& key) const { return std::lower_bound(std::cbegin(elems_), std::cend(elems_), key, CompareFirstAdapter<Comparator>()); }

	iterator upper_bound(const Key& key) { return std::upper_bound(std::begin(elems_), std::end(elems_), key, CompareFirstAdapter<Comparator>()); }
	const_iterator upper_bound(const Key& key) const { return std::upper_bound(std::cbegin(elems_), std::cend(elems_), key, CompareFirstAdapter<Comparator>()); }

	template<class It>
	void assign(It first, It last)
	{
		elems_.assign(first, last);
		std::sort(std::begin(elems_), std::end(elems_));
	}

	template<class Cont>
	void assign(const Cont& other) { assign(std::cbegin(other), std::cend(other)); }

	void clear() { elems_.clear(); }
	bool empty() const { return elems_.empty(); }
	void swap(assoc_vector& other) { elems_.swap(other.elems_); }
	allocator_type get_allocator() const { return elems_.get_allocator(); }

	size_type size() const { return elems_.size(); }
	size_type capacity() const { return elems_.capacity(); }

	void reserve(size_type sz) { elems_.reserve(sz); }
	void shrink_to_fit() { elems_.shrink_to_fit(); }

	const T& at_index(size_type index) const { return elems_.at(index); }

	auto begin() { return elems_.begin(); }
	auto end() { return elems_.end(); }

	auto cbegin() const { return elems_.cbegin(); }
	auto cend() const { return elems_.cend(); }

	auto begin() const { return cbegin(); }
	auto end() const { return cend(); }

	auto rbegin() { return elems_.rbegin(); }
	auto rend() { return elems_.rend(); }

	auto crbegin() const { return elems_.crbegin(); }
	auto crend() const { return elems_.crend(); }

	auto rbegin() const { return crbegin(); }
	auto rend() const { return crend(); }

	friend bool operator==(assoc_vector& left, assoc_vector& right) { return left.elems_ == right.elems_; }
	friend bool operator!=(assoc_vector& left, assoc_vector& right) { return left.elems_ != right.elems_; }

	friend bool operator<(assoc_vector& left, assoc_vector& right) { return left.elems_ < right.elems_; }
	friend bool operator>=(assoc_vector& left, assoc_vector& right) { return left.elems_ >= right.elems_; }

	friend bool operator>(assoc_vector& left, assoc_vector& right) { return left.elems_ > right.elems_; }
	friend bool operator<=(assoc_vector& left, assoc_vector& right) { return left.elems_ <= right.elems_; }

private:
	inner_container_type elems_;
};

#endif // !ASSOC_VECTOR_HPP