#pragma once

#include <optional>
#include <regex>

#include <mach/detail/validation/rules/ValidationRuleType.hpp>

namespace mach::detail::validation
{
    struct EmailRule {
        static constexpr ValidationRuleType Type = ValidationRuleType::Email;
    };

    struct LengthRule {
        static constexpr ValidationRuleType Type = ValidationRuleType::Length;
        std::optional<size_t> minLength;
        std::optional<size_t> maxLength;
    };

    struct UrlRule {
        static constexpr ValidationRuleType Type = ValidationRuleType::Url;
    };

    struct RegexRule {
        static constexpr ValidationRuleType Type = ValidationRuleType::Regex;
        std::regex pattern;
    };
}
