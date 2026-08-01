#pragma once

#include <concepts>
#include <optional>
#include <type_traits>

#include <mach/detail/core/TypeTraits.hpp>

namespace mach::detail::validation
{
    template <typename T>
    struct EqualRule {
        T value;
    };

    template <typename T>
    struct NotEqualRule {
        T value;
    };
}
