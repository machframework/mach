#pragma once

#include <concepts>
#include <type_traits>

namespace mach::detail::traits::dispatching
{
	template <typename T>
	struct is_reply : std::false_type {};

	template <typename T>
	struct is_reply<mach::Reply<T>> : std::true_type {};

	template <typename T>
	inline constexpr bool is_reply_v =
		is_reply<std::remove_cvref_t<T>>::value;
}
