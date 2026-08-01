#pragma once

#include <string_view>

#include <mach/detail/validation/rules/StringRules.hpp>

namespace mach::detail::validation
{
    bool validate(std::string_view value, const EmailRule& rule);

    bool validate(std::string_view value, const UrlRule& rule);

    template <typename T>
    bool validate(T value, const LengthRule& rule);

    //----------------------------------------------------------------
    // Implementation
    //----------------------------------------------------------------
}