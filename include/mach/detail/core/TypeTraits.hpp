#pragma once

namespace mach::detail::traits
{
    template <typename>
    inline constexpr bool always_false_v = false;
}
