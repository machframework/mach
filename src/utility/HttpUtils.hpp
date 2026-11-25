#pragma once

#include <string_view>

namespace mach::detail::http
{
    [[nodiscard]] constexpr bool isOws(char c) noexcept {
        return c == ' ' || c == '\t';
    }

    [[nodiscard]] constexpr std::string_view trimOws(std::string_view value) noexcept {
        while (!value.empty() && isOws(value.front())) {
            value.remove_prefix(1);
        }

        while (!value.empty() && isOws(value.back())) {
            value.remove_suffix(1);
        }

        return value;
    }
}
