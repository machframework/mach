#pragma once

#include <tuple>
#include <type_traits>

namespace mach::detail
{
    template <typename T, typename Tuple>
    struct tuple_contains;

    template <typename T, typename... Args>
    struct tuple_contains<T, std::tuple<Args...>>
        : std::bool_constant<
        (std::same_as<
            std::remove_cvref_t<T>,
            std::remove_cvref_t<Args>
        > || ...)
        >
    {};

    template <typename T, typename Tuple>
    inline constexpr bool tuple_contains_v =
        tuple_contains<T, Tuple>::value;
}
