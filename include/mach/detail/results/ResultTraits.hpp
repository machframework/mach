#pragma once

#include <concepts>
#include <type_traits>

#include <mach/results/Reply.hpp>

namespace mach::detail::results
{
    template <typename T>
    struct IsReply : std::false_type {};

    template <typename T>
    struct IsReply<mach::Reply<T>> : std::true_type {};

    template <typename T>
    concept ReplyResult =
        IsReply<std::remove_cvref_t<T>>::value;
}
