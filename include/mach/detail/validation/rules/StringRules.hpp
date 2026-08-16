#pragma once

#include <optional>
#include <regex>

#include <mach/detail/validation/rules/ValidationRuleType.hpp>

namespace mach::detail::validation
{
    struct EmailRule {
        static constexpr auto Type = ValidationRuleType::Email;
    };

    struct LengthRule {
        static constexpr auto Type = ValidationRuleType::Length;
        std::optional<size_t> minLength;
        std::optional<size_t> maxLength;
    };

    struct UrlRule {
        static constexpr auto Type = ValidationRuleType::Url;
    };

    struct RegexRule {
        static constexpr auto Type = ValidationRuleType::Regex;
        std::regex pattern;
    };
}
