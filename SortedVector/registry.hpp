#pragma once
#ifndef REGISTRY_HPP
#define REGISTRY_HPP

#include <vector>
#include <utility>
#include <optional>
#include <algorithm>

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


template<class T>
class registry {
	std::vector<std::pair<std::size_t, std::optional<T>>> elems_;
	std::size_t size_ = 0;
	std::size_t id_ = 0;

public:
	auto append(T element) -> std::size_t {
		const std::size_t currID = id_;
		elems_.emplace_back(currID, std::move(element));
		++size_;
		++id_;
		return currID;
	}

	void erase(std::size_t id) {
		const auto p = std::lower_bound(std::begin(elems_), std::end(elems_), id, ComparePairByFirst());
		if (p == std::end(elems_) || p->first != id) { return; }

		p->second.reset();
		--size_;

		if (size_ < (std::size(elems_) / 2)) {
			elems_.erase(std::remove_if(std::begin(elems_), std::end(elems_), [](const auto& e) { return !e.second; }), std::end(elems_));
		}
	}

	T* find(std::size_t id) {
		const auto p = std::lower_bound(std::begin(elems_), std::end(elems_), id, ComparePairByFirst());
		if (p == std::end(elems_) || p->first != id) { return nullptr; }

		return &(*p->second);
	}

	const T* find(std::size_t id) const {
		const auto p = std::lower_bound(std::cbegin(elems_), std::cend(elems_), id, ComparePairByFirst());
		if (p == std::cend(elems_) || p->first != id) { return nullptr; }

		return &(*p->second);
	}

	template<class F>
	void for_each(F f) {
		for (const auto& e : elems_) {
			if (e.second) f(*e.second);
		}
	}
};

#endif // !REGISTRY_HPP
