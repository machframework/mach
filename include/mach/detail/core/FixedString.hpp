#pragma once

#include <cstddef>
#include <string_view>

namespace mach::detail
{
    template <std::size_t N>
    struct FixedString {

        char data[N];

        constexpr FixedString(const char (&str)[N]) {
            for (std::size_t i = 0; i < N; ++i) {
                data[i] = str[i];
            }
        }

        constexpr operator std::string_view() const noexcept {
            return {data, sizeof(data) - 1};
        }
    };
}