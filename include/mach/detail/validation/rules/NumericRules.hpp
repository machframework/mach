#pragma once

#include <optional>

#include <mach/detail/core/TypeTraits.hpp>
#include <mach/detail/validation/rules/ValidationRuleType.hpp>

namespace mach::detail::validation
{
    template <traits::Numeric Number>
    struct RangeRule {
        static constexpr auto  Type = ValidationRuleType::Range;
        std::optional<Number> min;
        std::optional<Number> max;
    };

    template <traits::Numeric Number>
    struct MultipleOfRule {
        static constexpr auto  Type = ValidationRuleType::MultipleOf;
        Number factor;
    };
}
