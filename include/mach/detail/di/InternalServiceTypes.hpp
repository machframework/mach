#pragma once

#include <concepts>
#include <tuple>
#include <type_traits>

namespace mach::detail::routing
{
    class RouteTrie;
}

namespace mach::detail::dispatching
{
    class Dispatcher;
}

namespace mach::detail::di
{
    using ForbiddenDITypes =
        std::tuple<mach::detail::routing::RouteTrie, mach::detail::dispatching::Dispatcher>;

    template <typename T, typename Tuple>
    struct TupleContains;

    template <typename T, typename... Types>
    struct TupleContains<T, std::tuple<Types...>>
        : std::bool_constant<(std::same_as<std::remove_cvref_t<T>, Types> || ...)> {};

    template <typename T>
    inline constexpr bool isForbiddenDIType = TupleContains<T, ForbiddenDITypes>::value;
}
