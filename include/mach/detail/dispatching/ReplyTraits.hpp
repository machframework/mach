#pragma once

#include <concepts>
#include <type_traits>

namespace mach::detail::dispatching
{
	template <typename T>
	struct IsReply : std::false_type {};

	template <typename T>
	struct IsReply<mach::Reply<T>> : std::true_type {};

	template <typename T>
	inline constexpr bool IsReplyV =
		IsReply<std::remove_cvref_t<T>>::value;
}
