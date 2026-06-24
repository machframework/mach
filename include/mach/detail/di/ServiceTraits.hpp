#pragma once

#include <string>
#include <string_view>
#include <type_traits>

namespace mach::detail::di
{
    template <typename T>
    inline constexpr bool isValidServiceType =
        std::is_class_v<T> &&
        !std::is_const_v<T> &&
        !std::is_reference_v<T> &&
        !std::is_pointer_v<T> &&
        !std::is_same_v<T, std::string> &&
        !std::is_same_v<T, std::string_view>;
}
