#pragma once

#include <type_traits>

#include <mach/Reply.hpp>

namespace mach::detail::results::traits
{
    template <typename T>
    struct is_reply : std::false_type {};

    template <typename T>
    struct is_reply<mach::Reply<T>> : std::true_type {};

    template <typename T>
    concept ReplyResult = is_reply<std::remove_cvref_t<T>>::value;
}
