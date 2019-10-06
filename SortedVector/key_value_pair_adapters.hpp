#pragma once
#ifndef KEY_VALUE_PAIR_ADAPTERS
#define KEY_VALUE_PAIR_ADAPTERS

#include <utility>
#include <iterator>


template<class Comp>
struct CompareFirstAdapter {
	explicit CompareFirstAdapter() = default;

	template<class... Args>
	explicit CompareFirstAdapter(Args&&... args) : comp_{ std::forward<Args>(args)... } {}

	template<class First>
	bool operator()(const First& left, const First& right) const { return comp_(left, right); }

	template<class First, class Second>
	bool operator()(const std::pair<First, Second>& left, const std::pair<First, Second>& right) const { return this->operator()(left.first, right.first); }

	template<class First, class Second>
	bool operator()(const std::pair<First, Second>& left, const First& right) const { return this->operator()(left.first, right); }

	template<class First, class Second>
	bool operator()(const First& left, const std::pair<First, Second>& right) const { return this->operator()(left, right.first); }

private:
	Comp comp_;
};


template<class Key, class Value>
struct KeyValuePairRef {
	explicit KeyValuePairRef(std::pair<Key, Value>& keyValue) : first{ keyValue.first }, second{ keyValue.second } {}

	const Key& first;
	Value& second;
};

template<class Key, class Value>
struct KeyValuePairPtr {
	explicit KeyValuePairPtr(std::pair<Key, Value>& keyValue) : proxyPtr_{ keyValue } {}
	KeyValuePairRef<Key, Value>* operator->() { return &proxyPtr_; }

private:
	KeyValuePairRef<Key, Value> proxyPtr_;
};


template<class Comp = std::less<>>
struct ComparePairByFirst {
	template<class TypeFirst>
	bool operator()(const TypeFirst& left, const TypeFirst& right) const { return comp_(left, right); }

	template<class TypeFirst, class TypeSecond>
	bool operator()(const std::pair<TypeFirst, TypeSecond>& left, const std::pair<TypeFirst, TypeSecond>& right) const { return this->operator()(left.first, right.first); }

	template<class TypeFirst, class TypeSecond>
	bool operator()(const TypeFirst& left, const std::pair<TypeFirst, TypeSecond>& right) const { return this->operator()(left, right.first); }

	template<class TypeFirst, class TypeSecond>
	bool operator()(const std::pair<TypeFirst, TypeSecond>& left, const TypeFirst& right) const { return this->operator()(left.first, right); }

private:
	Comp comp_;
};

template<class Comp = std::less<>>
struct ComparePairBySecond {
	template<class TypeFirst>
	bool operator()(const TypeFirst& left, const TypeFirst& right) const { return comp_(left, right); }

	template<class TypeFirst, class TypeSecond>
	bool operator()(const std::pair<TypeFirst, TypeSecond>& left, const std::pair<TypeFirst, TypeSecond>& right) const { return this->operator()(left.second, right.second); }

	template<class TypeFirst, class TypeSecond>
	bool operator()(const TypeFirst& left, const std::pair<TypeFirst, TypeSecond>& right) const { return this->operator()(left, right.second); }

	template<class TypeFirst, class TypeSecond>
	bool operator()(const std::pair<TypeFirst, TypeSecond>& left, const TypeFirst& right) const { return this->operator()(left.second, right); }

private:
	Comp comp_;
};


#endif // !KEY_VALUE_PAIR_ADAPTERS
