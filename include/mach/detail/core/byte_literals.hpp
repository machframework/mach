#pragma once

#include <cstddef>
#include <limits>

namespace mach::detail
{
    consteval std::size_t checkedByteCount(
        unsigned long long value,
        unsigned long long multiplier) {
        const auto max = std::numeric_limits<std::size_t>::max();

        if (multiplier > max || value > max / multiplier) {
            throw "Mach byte-size literal exceeds std::size_t maximum";
        }

        return static_cast<std::size_t>(value * multiplier);
    }
}
