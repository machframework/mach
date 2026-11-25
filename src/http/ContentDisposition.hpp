#pragma once

#include <string>
#include <string_view>

namespace mach::detail::http
{
    [[nodiscard]] std::string formatContentDisposition(
        std::string_view disposition,
        std::string_view filename);
}
