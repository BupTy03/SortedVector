#pragma once
#ifndef TYPELIST_UTILS_HPP
#define TYPELIST_UTILS_HPP

#include <type_traits>

namespace impl {

	// indexed access impl
	template<std::size_t Index, class... Types>
	struct at_index_impl;

	template<class Head, class... Tail>
	struct at_index_impl<0, Head, Tail...> { using type = Head; };

	template<std::size_t Index, class Head, class... Tail>
	struct at_index_impl<Index, Head, Tail...> { using type = typename at_index_impl<Index - 1, Tail...>::type; };


	// finding index of type impl
	template<class Type, class... TypesPack>
	struct index_of;

	template<class Head, class... Tail>
	struct index_of<Head, Head, Tail...> : std::integral_constant<std::size_t, 0> {};

	template <typename Type, typename Head, typename... Tail>
	struct index_of<Type, Head, Tail...> : std::integral_constant<size_t, 1 + index_of<Type, Tail...>::value> {};

}

// indexed access
template<std::size_t Index, class... Types>
using at_index_t = typename impl::at_index_impl<Index, Types...>::type;

// finding index of type
template<class Type, class... TypesPack>
constexpr auto index_of_v = impl::index_of<Type, TypesPack...>::value;


#endif // !TYPELIST_UTILS_HPP
