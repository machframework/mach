#pragma once

namespace mach::detail::validation
{
    enum class ValidationRuleType {
        Email,
        Url,
        Length,
        Regex,
        Range,
        MultipleOf,
        Equal,
        NotEqual
    };

    template <typename Rule>
    inline constexpr bool IsUniqueRule = true;
}
