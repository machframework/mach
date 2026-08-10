#pragma once

#include <mach/detail/validation/rules/ValidationRuleType.hpp>

namespace mach::detail::validation
{
    template <typename T>
    struct EqualRule {
        static constexpr auto  Type = ValidationRuleType::Equal;
        T value;
    };

    template <typename T>
    struct NotEqualRule {
        static constexpr auto  Type = ValidationRuleType::NotEqual;
        T value;
    };
}
