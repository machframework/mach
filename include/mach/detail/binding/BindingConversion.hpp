#pragma once

#include <concepts>
#include <string>
#include <string_view>
#include <type_traits>

#include <mach/Json.hpp>

#include <mach/detail/serialization/StringConversion.hpp>
#include <mach/detail/core/TypeTraits.hpp>

namespace mach::detail::binding
{
    template <typename T>
    [[nodiscard]] T convertBindingValue(std::string_view value) {
        using ValueType = std::remove_cvref_t<T>;

        static_assert(
            std::same_as<T, ValueType>,
            "Mach error: convertBindingValue<T> requires a non-reference, non-cv-qualified type");

        if constexpr (detail::traits::isOptional_v<T>) {
            return T{convertBindingValue<typename T::value_type>(value)};
        } else if constexpr (
            std::same_as<T, std::string> || std::integral<T> || std::floating_point<T>
        ) {
            return mach::fromString<T>(value);
        } else {
            return mach::Json::parse(value).get<T>();
        }
    }
}
