#pragma once

#include <cstddef>
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

    [[nodiscard]] constexpr char toAsciiLowercase(char c) noexcept {
        if (c >= 'A' && c <= 'Z') {
            return static_cast<char>(c + ('a' - 'A'));
        }

        return c;
    }

    [[nodiscard]] constexpr bool equalsIgnoreCase(
        std::string_view lhs,
        std::string_view rhs) noexcept {
        if (lhs.size() != rhs.size()) {
            return false;
        }

        for (std::size_t i = 0; i < lhs.size(); ++i) {
            if (toAsciiLowercase(lhs[i]) != toAsciiLowercase(rhs[i])) {
                return false;
            }
        }

        return true;
    }

    [[nodiscard]] constexpr bool isTokenCharacter(char c) noexcept {
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')) {
            return true;
        }

        switch (c) {
        case '!':
        case '#':
        case '$':
        case '%':
        case '&':
        case '\'':
        case '*':
        case '+':
        case '-':
        case '.':
        case '^':
        case '_':
        case '`':
        case '|':
        case '~':
            return true;

        default:
            return false;
        }
    }

    [[nodiscard]] constexpr bool isToken(std::string_view value) noexcept {
        if (value.empty()) {
            return false;
        }

        for (const auto c : value) {
            if (!isTokenCharacter(c)) {
                return false;
            }
        }

        return true;
    }
}
