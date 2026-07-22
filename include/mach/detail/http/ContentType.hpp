#pragma once

#include <string_view>

namespace mach::detail::http
{
    [[nodiscard]]
    bool matchesMediaType(
        std::string_view contentType,
        std::string_view expectedMediaType) noexcept;

    [[nodiscard]]
    bool hasUnsupportedCharset(
        std::string_view contentType) noexcept;
}
