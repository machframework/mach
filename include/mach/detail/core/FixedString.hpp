#pragma once

#include <cstddef>

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
    };
}