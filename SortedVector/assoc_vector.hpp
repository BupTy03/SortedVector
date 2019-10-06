#pragma once
#ifndef ASSOC_VECTOR_HPP
#define ASSOC_VECTOR_HPP

#include "key_value_pair_adapters.hpp"
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
class assoc_vector {
	class iterator_adapter_impl;
	class const_iterator_adapter_impl;

public:
	using container_type = std::vector<std::pair<Key, T>, Allocator>;

	using value_type = typename container_type::value_type;
	using allocator_type = typename container_type::allocator_type;
	using size_type	= typename container_type::size_type;
	using difference_type = typename container_type::difference_type;

	using reference = KeyValuePairRef<Key, T>;
	using const_reference = typename container_type::const_reference;

	using pointer = KeyValuePairPtr<Key, T>;
	using const_pointer = typename container_type::const_pointer;

	using iterator = iterator_adapter_impl;
	using const_iterator = const_iterator_adapter_impl;

	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

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
			return this->emplace_hint(const_iterator(it), key, T())->second;
		}

		assert(it != std::end(elems_));
		return it->second;
	}
	const T& operator[](const Key& key) const { return at(key); }

	std::pair<iterator, bool> insert(const value_type& value)
	{
		const auto it = std::lower_bound(std::begin(elems_), std::end(elems_), value, CompareFirstAdapter<Comparator>());
		return std::make_pair(iterator(elems_.insert(it, value)), true);
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
			return iterator(elems_.insert(hint, value));
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
		return std::make_pair(iterator(elems_.insert(it, std::move(value))), true);
	}
	template<class... Args>
	iterator emplace_hint(const_iterator hint, Args&&... args)
	{
		auto elems_hint = hint.it_;

		if (elems_hint < std::cbegin(elems_) || elems_hint > std::cend(elems_)) {
			throw std::out_of_range{ "iterator is out of range" };
		}

		CompareFirstAdapter<Comparator> comp;
		const auto first = std::cbegin(elems_);
		const auto last = std::cend(elems_);

		value_type value(std::forward<Args>(args)...);

		if ((elems_hint == last && (elems_hint == first || !comp(value, *(std::prev(elems_hint))))) ||	// if no elems or value isn't less than prev element of last
			(elems_hint == first && comp(value, *elems_hint)) ||									// if value is less than first
			(!comp(value, *(std::prev(elems_hint))) && comp(value, *elems_hint)))					// if value is less than hint and not less than prev
		{
			return iterator(elems_.insert(elems_hint, std::move(value)));
		}

		return this->insert(value).first;
	}

	iterator erase(const value_type& value)
	{
		auto it = binary_find(elems_, value, CompareFirstAdapter<Comparator>());
		return iterator((it != std::end(elems_)) ? elems_.erase(it) : it);
	}
	void eraseAll(const value_type& value)
	{
		const auto itPair = std::equal_range(std::cbegin(elems_), std::cend(elems_), value, CompareFirstAdapter<Comparator>());
		iterator(elems_.erase(itPair.first, itPair.second));
	}

	iterator find(const Key& key) { return iterator(binary_find(elems_, key, CompareFirstAdapter<Comparator>())); }
	const_iterator find(const Key& key) const { return const_iterator(binary_find(elems_, key, CompareFirstAdapter<Comparator>())); }

	std::pair<iterator, iterator> equal_range(const Key& key) 
	{ 
		auto result = std::equal_range(std::begin(elems_), std::end(elems_), key, CompareFirstAdapter<Comparator>());
		return std::make_pair(iterator(result.first), iterator(result.second));
	}
	std::pair<const_iterator, const_iterator> equal_range(const Key& key) const { return const_iterator(std::equal_range(std::cbegin(elems_), std::cend(elems_), key, CompareFirstAdapter<Comparator>())); }

	iterator lower_bound(const Key& key) { return iterator(std::lower_bound(std::begin(elems_), std::end(elems_), key, CompareFirstAdapter<Comparator>())); }
	const_iterator lower_bound(const Key& key) const { return const_iterator(std::lower_bound(std::cbegin(elems_), std::cend(elems_), key, CompareFirstAdapter<Comparator>())); }

	iterator upper_bound(const Key& key) { return iterator(std::upper_bound(std::begin(elems_), std::end(elems_), key, CompareFirstAdapter<Comparator>())); }
	const_iterator upper_bound(const Key& key) const { return const_iterator(std::upper_bound(std::cbegin(elems_), std::cend(elems_), key, CompareFirstAdapter<Comparator>())); }

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
	T& at_index(size_type index) { return elems_.at(index); }

	iterator begin() { return iterator(elems_.begin()); }
	iterator end() { return iterator(elems_.end()); }

	const_iterator cbegin() const { return const_iterator(elems_.cbegin()); }
	const_iterator cend() const { return const_iterator(elems_.cend()); }

	const_iterator begin() const { return cbegin(); }
	const_iterator end() const { return cend(); }

	reverse_iterator rbegin() { return reverse_iterator(iterator(elems_.end())); }
	reverse_iterator rend() { return reverse_iterator(iterator(elems_.begin())); }

	const_reverse_iterator crbegin() const { return const_reverse_iterator(const_iterator(elems_.crbegin())); }
	const_reverse_iterator crend() const { return const_reverse_iterator(const_iterator(elems_.crend())); }

	const_reverse_iterator rbegin() const { return crbegin(); }
	const_reverse_iterator rend() const { return crend(); }

	friend bool operator==(assoc_vector& left, assoc_vector& right) { return left.elems_ == right.elems_; }
	friend bool operator!=(assoc_vector& left, assoc_vector& right) { return left.elems_ != right.elems_; }

	friend bool operator<(assoc_vector& left, assoc_vector& right) { return left.elems_ < right.elems_; }
	friend bool operator>=(assoc_vector& left, assoc_vector& right) { return left.elems_ >= right.elems_; }

	friend bool operator>(assoc_vector& left, assoc_vector& right) { return left.elems_ > right.elems_; }
	friend bool operator<=(assoc_vector& left, assoc_vector& right) { return left.elems_ <= right.elems_; }

private: // iterators implementation
	class iterator_adapter_impl {
		friend class assoc_vector<Key, T, Comparator, Allocator>;
		friend class const_iterator_adapter_impl;

	public:
		using iterator_category = std::random_access_iterator_tag;
		using value_type = typename container_type::iterator::value_type;
		using difference_type = typename container_type::iterator::difference_type;
		using pointer = KeyValuePairPtr<Key, T>;
		using reference = KeyValuePairRef<Key, T>;

	private:
		explicit iterator_adapter_impl(typename container_type::iterator it) : it_{ it } {}

	public:
		explicit iterator_adapter_impl() = default;

		iterator_adapter_impl& operator++() { ++it_; return *this; }
		iterator_adapter_impl operator++(int) { auto result = *this; ++(*this); return result; }

		iterator_adapter_impl& operator--() { --it_; return *this; }
		iterator_adapter_impl operator--(int) { auto result = *this; --(*this); return result; }

		iterator_adapter_impl& operator+=(difference_type shift) { it_ += shift; return *this; }
		iterator_adapter_impl operator+(difference_type shift) const { auto result = *this; result += shift; return result; }

		iterator_adapter_impl& operator-=(difference_type shift) { it_ -= shift; return *this; }
		iterator_adapter_impl operator-(difference_type shift) const { auto result = *this; result -= shift; return result; }

		difference_type operator-(iterator_adapter_impl other) const { return it_ - other.it_; }

		reference operator*() const { return reference{ *it_ }; }
		pointer operator->() const { return pointer{ *it_ }; }
		reference operator[](difference_type n) const { return *(*this + n); }

		bool operator<(iterator_adapter_impl other) { return (*this - other) < 0; }
		bool operator>(iterator_adapter_impl other) { return (*this - other) > 0; }

		bool operator==(iterator_adapter_impl other) { return (*this - other) == 0; }
		bool operator!=(iterator_adapter_impl other) { return !(*this == other); }

		bool operator<=(iterator_adapter_impl other) { return !(*this > other); }
		bool operator>=(iterator_adapter_impl other) { return !(*this < other); }

	private:
		typename container_type::iterator it_;
	};

	class const_iterator_adapter_impl {
		friend class assoc_vector<Key, T, Comparator, Allocator>;

	public:
		using iterator_category = std::random_access_iterator_tag;
		using value_type = typename container_type::const_iterator::value_type;
		using difference_type = typename container_type::const_iterator::difference_type;
		using pointer = typename container_type::const_iterator::pointer;
		using reference = typename container_type::const_iterator::reference;

	private:
		explicit const_iterator_adapter_impl(typename container_type::const_iterator it) : it_{ it } {}

	public:
		explicit const_iterator_adapter_impl() = default;
		explicit const_iterator_adapter_impl(iterator_adapter_impl it) : it_{it.it_} { }

		const_iterator_adapter_impl& operator++() { ++it_; return *this; }
		const_iterator_adapter_impl operator++(int) { auto result = *this; ++(*this); return result; }

		const_iterator_adapter_impl& operator--() { --it_; return *this; }
		const_iterator_adapter_impl operator--(int) { auto result = *this; --(*this); return result; }

		const_iterator_adapter_impl& operator+=(difference_type shift) { it_ += shift; return *this; }
		const_iterator_adapter_impl operator+(difference_type shift) const { auto result = *this; result += shift; return result; }

		const_iterator_adapter_impl& operator-=(difference_type shift) { it_ -= shift; return *this; }
		const_iterator_adapter_impl operator-(difference_type shift) const { auto result = *this; result -= shift; return result; }

		difference_type operator-(const_iterator_adapter_impl other) const { return it_ - other.it_; }

		reference operator*() const { return reference{ *it_ }; }
		pointer operator->() const { return &(*it_); }
		reference operator[](difference_type n) const { return *(*this + n); }

		bool operator<(const_iterator_adapter_impl other) { return (*this - other) < 0; }
		bool operator>(const_iterator_adapter_impl other) { return (*this - other) > 0; }

		bool operator==(const_iterator_adapter_impl other) { return (*this - other) == 0; }
		bool operator!=(const_iterator_adapter_impl other) { return !(*this == other); }

		bool operator<=(const_iterator_adapter_impl other) { return !(*this > other); }
		bool operator>=(const_iterator_adapter_impl other) { return !(*this < other); }

	private:
		typename container_type::const_iterator it_;
	};

private:
	container_type elems_;
};

#endif // !ASSOC_VECTOR_HPP