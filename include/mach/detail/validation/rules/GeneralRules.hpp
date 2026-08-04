#pragma once

#include <concepts>
#include <optional>
#include <type_traits>

#include <mach/detail/core/TypeTraits.hpp>
#include <mach/detail/validation/rules/ValidationRuleType.hpp>

namespace mach::detail::validation
{
    template <typename T>
    struct EqualRule {
        static constexpr ValidationRuleType Type = ValidationRuleType::Equal;
        T value;
    };

    template <typename T>
    struct NotEqualRule {
        static constexpr ValidationRuleType Type = ValidationRuleType::NotEqual;
        T value;
    };
}
