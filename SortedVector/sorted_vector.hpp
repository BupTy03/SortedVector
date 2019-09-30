#pragma once
#ifndef SORTED_VECTOR_HPP
#define SORTED_VECTOR_HPP

#include "typelist_utils.hpp"

#include <vector>
#include <algorithm>
#include <cassert>

template<class ForwardIt, class T, class Compare = std::less<>>
ForwardIt binary_find(ForwardIt first, ForwardIt last, const T& value, Compare comp = {})
{
	first = std::lower_bound(first, last, value, comp);
	return first != last && !comp(value, *first) ? first : last;
}

template<class Container, class T, class Compare = std::less<>>
auto binary_find(Container& cont, const T& value, Compare comp = {})
{
	return binary_find(std::begin(cont), std::end(cont), value, comp);
}

template<class Container, class T, class Compare = std::less<>>
auto binary_find(const Container& cont, const T& value, Compare comp = {})
{
	return binary_find(std::cbegin(cont), std::cend(cont), value, comp);
}

template<class T, class Allocator = std::allocator<T>, class... Comparators>
struct sorted_vector;

template<class T, class Allocator, class Comparator>
struct sorted_vector<T, Allocator, Comparator> {
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


template<class T, class Allocator, class... Comparators>
struct sorted_vector {
	template<class CurrComp>
	class const_iterator_impl;

	template<class CurrComp>
	friend class const_iterator_impl;

	using inner_container_type = std::vector<T, Allocator>;

	using value_type = typename inner_container_type::value_type;
	using allocator_type = typename inner_container_type::allocator_type;
	using size_type = typename inner_container_type::size_type;
	using difference_type = typename inner_container_type::difference_type;

	using const_reference = typename inner_container_type::const_reference;
	using const_pointer = typename inner_container_type::const_pointer;

	template<class CurrComp>
	using const_iterator = const_iterator_impl<CurrComp>;

	template<class CurrComp>
	using const_reverse_iterator = std::reverse_iterator<const_iterator<CurrComp>>;


public:
	explicit sorted_vector() : sortedIndexes_{ sizeof...(Comparators) } {}

	auto insert(const T& val) { elems_.push_back(val); update_sorted(); }
	auto insert(T&& val) { elems_.push_back(std::move(val)); update_sorted(); }

	template<class... Args>
	auto emplace(Args&&... args) { elems_.emplace_back(std::forward<Args>(args)...); update_sorted(); }

	template<std::size_t index, typename VT>
	T findByComparatorOrDefault(const VT& value, const T& defaultValue) const
	{
		at_index_t<index, Comparators...> comp;
		const auto compareByValues = [this, comp](std::size_t leftIndex, const VT& value) {
			std::cout << leftIndex << std::endl;
			return comp(elems_.at(leftIndex), value);
		};

		const auto foundIndexIt = std::lower_bound(std::cbegin(sortedIndexes_.at(index)), std::cend(sortedIndexes_.at(index)), value, compareByValues);
		return (foundIndexIt != std::cend(sortedIndexes_.at(index)) && !comp(elems_.at(*foundIndexIt), value)) ? elems_.at(*foundIndexIt) : defaultValue;
	}

	template<class Comp, typename VT, typename = std::enable_if_t<!std::is_same_v<Comp, std::size_t>>>
	T findByComparatorOrDefault(const VT& value, const T& defaultValue) const  { return findByComparatorOrDefault<index_of_v<Comp, Comparators...>>(value, defaultValue); }

private:
	template<class CurrComp>
	bool for_every_of(std::size_t& currIndex)
	{
		auto& currIndexes = sortedIndexes_.at(currIndex);

		const auto compareByValues = [this](std::size_t left, std::size_t right) {
			CurrComp comp;
			return comp(elems_.at(left), elems_.at(right));
		};

		const auto currElemIndex = elems_.size() - 1;
		currIndexes.insert(std::lower_bound(std::cbegin(currIndexes), std::cend(currIndexes), currElemIndex, compareByValues), currElemIndex);
		++currIndex;
		return true;
	}

	void update_sorted() { insert_to_sorted(elems_.back()); }

	void insert_to_sorted(const T& value)
	{
		std::size_t currCompIndex = 0;
		bool do_this[]{ for_every_of<Comparators>(currCompIndex)... };
	}

private: // iterators

	template<class CurrComp>
	class const_iterator_impl : public std::iterator<std::random_access_iterator_tag, T, std::ptrdiff_t, const T*, const T&> {
		using base_type = std::iterator<std::random_access_iterator_tag, T, std::ptrdiff_t, const T*, const T&>;

		friend class sorted_vector<T, Allocator, Comparators...>;
	private:
		explicit constexpr const_iterator_impl(const inner_container_type& pElems, std::vector<std::size_t>::const_iterator indexIt)
			: pElems_{ &pElems }, currIndexIt_{ indexIt } { assert(pElems_ != nullptr); }

	public:
		using iterator_category = typename base_type::iterator_category;
		using value_type = typename base_type::value_type;
		using difference_type = typename base_type::difference_type;
		using pointer = typename base_type::pointer;
		using reference = typename base_type::reference;

	public:
		explicit constexpr const_iterator_impl() = default;

		//constexpr const_iterator_impl(const const_iterator_impl&) = default;
		//constexpr const_iterator_impl& operator=(const const_iterator_impl&) = default;

		//constexpr const_iterator_impl(const_iterator_impl&&) = default;
		//constexpr const_iterator_impl& operator=(const_iterator_impl&&) = default;

		constexpr const_iterator_impl& operator++() { ++currIndexIt_; return *this; }
		constexpr const_iterator_impl operator++(int) { auto result = *this; ++(*this); return result; }

		constexpr const_iterator_impl& operator--() { --currIndexIt_; return *this; }
		constexpr const_iterator_impl operator--(int) { auto result = *this; --(*this); return result; }

		constexpr const_iterator_impl& operator+=(difference_type shift) { currIndexIt_ += shift; return *this; }
		constexpr const_iterator_impl operator+(difference_type shift) const { auto result = *this; result += shift; return result; }

		constexpr const_iterator_impl& operator-=(difference_type shift) { currIndexIt_ -= shift; return *this; }
		constexpr const_iterator_impl operator-(difference_type shift) const { auto result = *this; result -= shift; return result; }

		constexpr difference_type operator-(const_iterator_impl other) const { return currIndexIt_ - other.currIndexIt_; }

		constexpr reference operator*() const { return pElems_->at(*currIndexIt_); }
		constexpr pointer operator->() const { return &(pElems_->at(*currIndexIt_)); }
		constexpr reference operator[](difference_type n) const { return *(*this + n); }

		constexpr bool operator<(const_iterator_impl other) { return (*this - other) < 0; }
		constexpr bool operator>(const_iterator_impl other) { return (*this - other) > 0; }

		constexpr bool operator==(const_iterator_impl other) { return (*this - other) == 0; }
		constexpr bool operator!=(const_iterator_impl other) { return !(*this == other); }

		constexpr bool operator<=(const_iterator_impl other) { return !(*this > other); }
		constexpr bool operator>=(const_iterator_impl other) { return !(*this < other); }

	private:
		const inner_container_type* pElems_ = nullptr;
		std::vector<std::size_t>::const_iterator currIndexIt_;
	};

private:
	inner_container_type elems_;
	std::vector<std::vector<std::size_t>> sortedIndexes_;
};

#endif // !SORTED_VECTOR_HPP