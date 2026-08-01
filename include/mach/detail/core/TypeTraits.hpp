#pragma once

namespace mach::detail::traits
{
    template <typename>
    inline constexpr bool always_false_v = false;

    template <typename T>
    concept Numeric =
        !std::same_as<std::remove_cv_t<T>, bool> && (std::integral<T> || std::floating_point<T>);
}
