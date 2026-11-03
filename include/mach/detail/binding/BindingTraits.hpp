#pragma once

#include <type_traits>

#include <mach/binding/Body.hpp>
#include <mach/binding/Param.hpp>
#include <mach/binding/Query.hpp>

namespace mach::detail::binding
{
    template <typename T>
    struct BindingTraits {
        static constexpr bool is_binding = false;
    };

    template <typename T>
    struct BindingTraits<Body<T>> {
        static constexpr bool is_binding = true;
        using value_type = T;
    };

    template <typename T, FixedString Name>
    struct BindingTraits<Param<T, Name>> {
        static constexpr bool is_binding = true;
        using value_type = T;
        static constexpr auto name = Name;
    };

    template <typename T, FixedString Name>
    struct BindingTraits<Query<T, Name>> {
        static constexpr bool is_binding = true;
        using value_type = T;
        static constexpr auto name = Name;
    };
}