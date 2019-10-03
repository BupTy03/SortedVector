#pragma once
#ifndef SORTED_VECTOR_HPP
#define SORTED_VECTOR_HPP

#include "typelist_utils.hpp"

#include <vector>
#include <algorithm>
#include <cassert>
#include <set>


template<class ForwardIt, class T, class Compare = std::less<>>
ForwardIt binary_find(ForwardIt first, ForwardIt last, const T& value, Compare comp = {})
{
	first = std::lower_bound(first, last, value, comp);
	return first != last && !comp(value, *first) ? first : last;
}

template<class Container, class T, class Compare = std::less<>>
auto binary_find(Container& cont, const T& value, Compare comp = {}){ return binary_find(std::begin(cont), std::end(cont), value, comp); }

template<class Container, class T, class Compare = std::less<>>
auto binary_find(const Container& cont, const T& value, Compare comp = {}){ return binary_find(std::cbegin(cont), std::cend(cont), value, comp); }

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

private: // local traits
	template<class Comp>
	using contains_comp = std::enable_if_t<contains_v<Comp, Comparators...>>;

	template<class Comp>
	static constexpr auto index_of_comp = index_of_v<Comp, Comparators...>;

	static constexpr auto count_comparators = sizeof...(Comparators);

public:
	explicit sorted_vector() : sortedIndexes_{ count_comparators } {}

	void insert(const T& val) { elems_.push_back(val); update_sorted(); }
	void insert(T&& val) { elems_.push_back(std::move(val)); update_sorted(); }

	template<class... Args>
	void emplace(Args&&... args) { elems_.emplace_back(std::forward<Args>(args)...); update_sorted(); }

	bool erase(const T& value)
	{
		const auto indexes_to_erase = find_elements_indexes(value);
		if (indexes_to_erase.empty()) {
			return false;
		}

		const auto erasedIndex = *(indexes_to_erase.begin());
		elems_.erase(std::cbegin(elems_) + erasedIndex);
		for (auto& indexes : sortedIndexes_) {
			indexes.erase(std::find(std::cbegin(indexes), std::cend(indexes), erasedIndex));
			for (auto& index : indexes) {
				if (index > erasedIndex) {
					--index;
				}
			}
		}
		return true;
	}
	bool eraseAll(const T& value)
	{
		const auto indexes_to_erase = find_elements_indexes(value);
		size_type shift = 0;
		for (auto index_to_erase : indexes_to_erase) {
			const auto currIndexToErase = index_to_erase - shift;
			elems_.erase(std::cbegin(elems_) + currIndexToErase);

			for (auto& sortedIndex : sortedIndexes_) {
				sortedIndex.erase(std::find(std::cbegin(sortedIndex), std::cend(sortedIndex), currIndexToErase));
				for (auto& index : sortedIndex) {
					if (index > index_to_erase) {
						--index;
					}
				}
			}

			++shift;
		}

		return (shift == 0);
	}

	void reserve(size_type space)
	{
		if (space <= elems_.capacity()) {
			return;
		}

		elems_.reserve(space);
		for (auto& indexes : sortedIndexes_) {
			indexes.reserve(space);
		}
	}
	size_type capacity() { return elems_.capacity(); }

	void shrink_to_fit()
	{
		if (capacity() == size()) {
			return;
		}

		elems_.shrink_to_fit();
		for (auto& indexes : sortedIndexes_) {
			indexes.shrink_to_fit();
		}
	}

	size_type size() const { return elems_.size(); }
	bool empty() const { return elems_.empty(); }

	template<class It>
	void assign(It first, It last) { std::for_each(first, last, [this](const T& val) { insert(val); }); }

	template<class Cont>
	void assign(const Cont& other) { assign(std::cbegin(other), std::cend(other)); }

	template<class Comp, typename VT, typename = contains_comp<Comp>>
	auto find(const VT& value) const -> const_iterator<Comp>
	{
		Comp comp;
		const auto compareByValues = [this, comp](size_type leftIndex, const VT& value) {
			return comp(elems_.at(leftIndex), value);
		};

		const auto& currSorted = sortedIndexes_.at(index_of_comp<Comp>);

		const auto foundIndexIt = binary_find(currSorted, value, compareByValues);
		const auto lastIndexIt = std::cend(currSorted);

		return (foundIndexIt != lastIndexIt)
			? const_iterator<Comp>(elems_, foundIndexIt)
			: const_iterator<Comp>(elems_, lastIndexIt);
	}

	template<class Comp, typename VT, typename = contains_comp<Comp>>
	auto findAll(const VT& value) const -> std::pair<const_iterator<Comp>, const_iterator<Comp>>
	{
		const auto foundLowerBound = find<Comp>(value);
		if (foundLowerBound == cend<Comp>()) {
			return std::make_pair(foundLowerBound, foundLowerBound);
		}

		const auto foundIndexIt = foundLowerBound.currIndexIt_;
		return std::make_pair(
			// first in range of equal elems
			const_iterator<Comp>(elems_, foundIndexIt),
			// last(going after the last) in range of equal elems
			const_iterator<Comp>(elems_, std::upper_bound(foundIndexIt, std::cend(sortedIndexes_.at(index_of_comp<Comp>)), *foundIndexIt)));
	}

	template<class Comp, typename VT, typename = contains_comp<Comp>>
	const T& at(size_type index) const { return elems_.at((sortedIndexes_.at(index_of_comp<Comp>)).at(index)); }

	template<class Comp, typename = contains_comp<Comp>>
	auto cbegin() const { return const_iterator<Comp>(elems_, std::cbegin(sortedIndexes_.at(index_of_v<Comp, Comparators...>))); }

	template<class Comp, typename = contains_comp<Comp>>
	auto cend() const { return const_iterator<Comp>(elems_, std::cend(sortedIndexes_.at(index_of_v<Comp, Comparators...>))); }

	template<class Comp> auto begin() const { return cbegin<Comp>(); }
	template<class Comp> auto end() const { return cend<Comp>(); }

	template<class Comp>
	auto crbegin() const -> std::reverse_iterator<const_iterator<Comp>> { return std::reverse_iterator<const_iterator<Comp>>(cend<Comp>()); }

	template<class Comp>
	auto crend() const -> std::reverse_iterator<const_iterator<Comp>> { return std::reverse_iterator<const_iterator<Comp>>(cbegin<Comp>()); }

	template<class Comp>
	auto rbegin() const { return crbegin<Comp>(); }

	template<class Comp>
	auto rend() const { return crend<Comp>(); }

	void swap(sorted_vector& other)
	{
		std::swap(elems_, other.elems_);
		std::swap(sortedIndexes_, other.sortedIndexes_);
	}

	template<class Comp, class It>
	int compare(It first, It last) const
	{ 
		Comp comp;

		auto thisFirst = begin<Comp>();
		const auto thisLast = end<Comp>();
		for (; (first != last) && (thisFirst != thisLast); ++first, ++thisFirst) {
			if (comp(*first, *thisFirst)) return -1;
			if (comp(*thisFirst, *first)) return 1;
		}

		return (first == last && thisFirst == thisLast)
			? 0
			: (first == last && thisFirst != thisLast) ? -1 : 1;
	}

	template<class Comp, class Container>
	int compare(const Container& cont) const { return compare<Comp>(std::cbegin(cont), std::cend(cont)); }

	bool operator==(const sorted_vector& other) const
	{
		if (size() != other.size()) {
			return false;
		}

		auto firstLeftIndexesIt = sortedIndexes_.cbegin();
		const auto lastLeftIndexesIt = sortedIndexes_.cend();
		if (firstLeftIndexesIt == lastLeftIndexesIt) {
			return true;
		}

		for (auto currIndxIt = firstLeftIndexesIt->cbegin(); currIndxIt != firstLeftIndexesIt->cend(); ++currIndxIt) {
			if (elems_.at(*currIndxIt) != other.elems_.at(*currIndxIt)) return false;
		}

		++firstLeftIndexesIt;
		for (auto firstRightIndexesIt = std::next(other.sortedIndexes_.cbegin()); firstLeftIndexesIt != lastLeftIndexesIt; ++firstLeftIndexesIt, ++firstRightIndexesIt) {
			if (!std::equal(firstLeftIndexesIt->cbegin(), firstLeftIndexesIt->cend(), firstRightIndexesIt->cbegin())) {
				return false;
			}
		}

		return true;
	}
	bool operator!=(const sorted_vector& other) const { return !(*this == other); }

private:

	template<class CompType>
	struct ByValueComparatorAdaptor {
		explicit ByValueComparatorAdaptor(const inner_container_type& valuesContext)
			: pValuesContext_{ &valuesContext } {}

		bool operator()(size_type left, size_type right) const { return comp_(pValuesContext_->at(left), pValuesContext_->at(right)); }
		bool operator()(size_type left, const T& right) const { return comp_(pValuesContext_->at(left), right); }
		bool operator()(const T& left, size_type right) const { return comp_(left, pValuesContext_->at(right)); }

	private:
		const inner_container_type* pValuesContext_ = nullptr;
		CompType comp_;
	};

	template<class CurrComp>
	bool for_every_of()
	{
		const auto currElemIndex = elems_.size() - 1;
		auto& currIndexes = sortedIndexes_.at(index_of_comp<CurrComp>);
		currIndexes.insert(std::lower_bound(std::cbegin(currIndexes), std::cend(currIndexes), currElemIndex, ByValueComparatorAdaptor<CurrComp>(elems_)), currElemIndex);

		return true;
	}

	void update_sorted() { insert_to_sorted(elems_.back()); }
	void insert_to_sorted(const T& value) { bool do_this[]{ for_every_of<Comparators>()... }; }

	template<class CurrComp>
	auto find_by(const T& value) -> std::pair<typename std::vector<size_type>::iterator, typename std::vector<size_type>::iterator>
	{
		return binary_find_range(sortedIndexes_.at(index_of_comp<CurrComp>), value, ByValueComparatorAdaptor<CurrComp>(elems_));
	}

	auto find_elements_indexes(const T& value) -> std::set<size_type>
	{
		using iter_type = typename std::vector<size_type>::iterator;
		using iters_pair = std::pair<iter_type, iter_type>;

		iters_pair pairs_arr[] = { find_by<Comparators>(value)... };

		std::set<size_type> result;
		for (const auto currIters : pairs_arr) {
			for (auto it = currIters.first; it != currIters.second; ++it) {
				if (elems_.at(*it) == value) {
					result.emplace(*it);
				}
			}
		}

		return result;
	}

private: // iterators
	template<class CurrComp>
	class const_iterator_impl : public std::iterator<std::random_access_iterator_tag, value_type, difference_type, const_pointer, const_reference> {
		using base_type = std::iterator<std::random_access_iterator_tag, value_type, difference_type, const_pointer, const_reference>;

		friend class sorted_vector<T, Allocator, Comparators...>;
	private:
		explicit constexpr const_iterator_impl(const inner_container_type& pElems, typename std::vector<size_type>::const_iterator indexIt)
			: pElems_{ &pElems }, currIndexIt_{ indexIt } { assert(pElems_ != nullptr); }

	public:
		using iterator_category = typename base_type::iterator_category;
		using value_type = typename base_type::value_type;
		using difference_type = typename base_type::difference_type;
		using pointer = typename base_type::pointer;
		using reference = typename base_type::reference;

	public:
		explicit constexpr const_iterator_impl() = default;

		constexpr const_iterator_impl& operator++() { ++currIndexIt_; return *this; }
		constexpr const_iterator_impl operator++(int) { auto result = *this; ++(*this); return result; }

		constexpr const_iterator_impl& operator--() { --currIndexIt_; return *this; }
		constexpr const_iterator_impl operator--(int) { auto result = *this; --(*this); return result; }

		constexpr const_iterator_impl& operator+=(difference_type shift) { currIndexIt_ += shift; return *this; }
		constexpr const_iterator_impl operator+(difference_type shift) const { auto result = *this; result += shift; return result; }

		constexpr const_iterator_impl& operator-=(difference_type shift) { currIndexIt_ -= shift; return *this; }
		constexpr const_iterator_impl operator-(difference_type shift) const { auto result = *this; result -= shift; return result; }

		constexpr difference_type operator-(const const_iterator_impl other) const { return currIndexIt_ - other.currIndexIt_; }

		constexpr reference operator*() const { return pElems_->at(*currIndexIt_); }
		constexpr pointer operator->() const { return &(pElems_->at(*currIndexIt_)); }
		constexpr reference operator[](difference_type n) const { return *(*this + n); }

		constexpr bool operator<(const_iterator_impl other) const { return (*this - other) < 0; }
		constexpr bool operator>(const_iterator_impl other) const { return (*this - other) > 0; }

		constexpr bool operator==(const_iterator_impl other) const { return (*this - other) == 0; }
		constexpr bool operator!=(const_iterator_impl other) const { return !(*this == other); }

		constexpr bool operator<=(const_iterator_impl other) const { return !(*this > other); }
		constexpr bool operator>=(const_iterator_impl other) const { return !(*this < other); }

	private:
		const inner_container_type* pElems_ = nullptr;
		typename std::vector<size_type>::const_iterator currIndexIt_;
	};

private:
	inner_container_type elems_;
	std::vector<std::vector<size_type>> sortedIndexes_;
};

template<class T, class... Comparators>
using SortedCollection = sorted_vector<T, std::allocator<T>, Comparators...>;

#endif // !SORTED_VECTOR_HPP