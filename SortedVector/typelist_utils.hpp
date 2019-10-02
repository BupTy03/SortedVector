#pragma once
#ifndef TYPELIST_UTILS_HPP
#define TYPELIST_UTILS_HPP

#include <type_traits>

namespace impl {

	// indexed access impl
	template<std::size_t Index, class... TypesPack>
	struct at_index_impl;

	template<class Head, class... Tail>
	struct at_index_impl<0, Head, Tail...> { using type = Head; };

	template<std::size_t Index, class Head, class... Tail>
	struct at_index_impl<Index, Head, Tail...> { using type = typename at_index_impl<Index - 1, Tail...>::type; };

	// finding index of type impl
	template<class Type, class... TypesPack>
	struct index_of_impl;

	template<class Head, class... Tail>
	struct index_of_impl<Head, Head, Tail...> : std::integral_constant<std::size_t, 0> {};

	template <class Type, class Head, class... Tail>
	struct index_of_impl<Type, Head, Tail...> : std::integral_constant<size_t, 1 + index_of_impl<Type, Tail...>::value> {};

	// contains impl
	template<class Type, class... TypesPack>
	struct contains_impl;

	template<class Head, class... Tail>
	struct contains_impl<Head, Head, Tail...> : std::integral_constant<bool, true> {};

	template<class Type, class Head, class... Tail>
	struct contains_impl<Type, Head, Tail...> : std::integral_constant<bool, contains_impl<Type, Tail...>::value> {};
}

// indexed access
template<std::size_t Index, class... TypesPack>
using at_index_t = typename impl::at_index_impl<Index, TypesPack...>::type;

// finding index of type
template<class Type, class... TypesPack>
constexpr auto index_of_v = impl::index_of_impl<Type, TypesPack...>::value;

// contains
template<class Type, class... TypesPack>
constexpr auto contains_v = impl::contains_impl<Type, TypesPack...>::value;

// count of types
template<class... TypesPack>
constexpr auto count_of_v = sizeof...(TypesPack);


#endif // !TYPELIST_UTILS_HPP
