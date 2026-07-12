#pragma once

#include <string>
#include <string_view>
#include <type_traits>

namespace mach::detail::di
{
    template <typename T, typename = void>
    inline constexpr bool isCompleteType = false;

    template <typename T>
    inline constexpr bool isCompleteType<
        T,
        std::void_t<decltype(sizeof(T))>
    > = true;

    template <typename T>
    concept CompleteType = requires {
        sizeof(T);
    };

    template <typename T>
    inline constexpr bool isValidServiceType =
        CompleteType<T> &&
        std::same_as<T, std::remove_cvref_t<T>>&&
        std::is_class_v<T> &&
        !std::same_as<T, std::string> &&
        !std::same_as<T, std::string_view>;

    template <typename T, typename... Deps>
    inline constexpr bool containsType =
        (std::same_as<T, Deps> || ...);

    template <typename...>
    inline constexpr bool areUniqueTypes = true;

    template <typename First, typename... Rest>
    inline constexpr bool areUniqueTypes<First, Rest...> =
        !containsType<First, Rest...>&&
        areUniqueTypes<Rest...>;
}
