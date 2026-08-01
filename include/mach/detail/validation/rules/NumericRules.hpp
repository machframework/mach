#pragma once

#include <concepts>
#include <optional>
#include <type_traits>

#include <mach/detail/core/TypeTraits.hpp>

namespace mach::detail::validation
{
    template <traits::Numeric Number>
    struct RangeRule {
        std::optional<Number> min;
        std::optional<Number> max;
    };

    template <traits::Numeric Number>
    struct MultipleOfRule {
        Number factor;
    };

    template <traits::Numeric Number>
    struct EqualRule {
        Number value;
    };

    template <traits::Numeric Number>
    struct NotEqualRule {
        Number value;
    };
}
