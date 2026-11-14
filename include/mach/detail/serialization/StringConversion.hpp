#pragma once

#include <charconv>
#include <concepts>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <type_traits>

namespace mach
{
    template <typename T>
    [[nodiscard]] T fromString(std::string_view value) {
        using ValueType = std::remove_cvref_t<T>;

        static_assert(
            std::same_as<T, ValueType>,
            "Mach error: fromString<T> requires a non-reference, non-cv-qualified type");

        if constexpr (std::same_as<ValueType, std::string>) {
            return std::string(value);
        } else if constexpr (std::integral<ValueType> && !std::same_as<ValueType, bool>) {
            ValueType result{};

            const auto [ptr, ec] =
                std::from_chars(value.data(), value.data() + value.size(), result);

            if (ec != std::errc{} || ptr != value.data() + value.size()) {
                throw std::invalid_argument("Mach error: failed to convert string to integer");
            }

            return result;
        } else if constexpr (std::floating_point<ValueType>) {
            ValueType result{};

            const auto [ptr, ec] =
                std::from_chars(value.data(), value.data() + value.size(), result);

            if (ec != std::errc{} || ptr != value.data() + value.size()) {
                throw std::invalid_argument(
                    "Mach error: failed to convert string to floating point");
            }

            return result;
        } else if constexpr (std::same_as<ValueType, bool>) {
            if (value == "true" || value == "1") {
                return true;
            }

            if (value == "false" || value == "0") {
                return false;
            }

            throw std::invalid_argument("Mach error: failed to convert string to bool");
        } else {
            static_assert(std::same_as<ValueType, void>, "Mach error: unsupported target type");
        }
    }
}
