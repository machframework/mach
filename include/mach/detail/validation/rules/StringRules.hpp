#pragma once

#include <optional>

namespace mach::detail::validation
{
    struct EmailRule {
    };

    struct LengthRule {
        std::optional<size_t> minLength;
        std::optional<size_t> maxLength;
    };

    struct UrlRule {
    };
}
