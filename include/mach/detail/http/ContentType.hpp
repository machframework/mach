#pragma once

#include <string_view>

namespace mach::detail::http
{
    bool matchesMediaType(
        std::string_view contentType,
        std::string_view expectedMediaType) noexcept;

    bool hasUnsupportedCharset(std::string_view contentType) noexcept;
}
