#pragma once

namespace mach
{
    template <typename T>
    T fromString(std::string_view value) {
        if constexpr (std::same_as<T, std::string>) {
            return std::string(value);
        }
        else if constexpr (std::integral<T> && !std::same_as<T, bool>) {
            T result{};
            auto [ptr, ec] = std::from_chars(value.data(), value.data() + value.size(), result);

            if (ec != std::errc{} || ptr != value.data() + value.size()) {
                throw std::invalid_argument("Mach error: failed to convert string to integer");
            }

            return result;
        }
        else if constexpr (std::floating_point<T>) {
            T result{};
            auto [ptr, ec] = std::from_chars(value.data(), value.data() + value.size(), result);

            if (ec != std::errc{} || ptr != value.data() + value.size()) {
                throw std::invalid_argument("Mach error: failed to convert string to floating point");
            }

            return result;
        }
        else if constexpr (std::same_as<T, bool>) {
            if (value == "true" || value == "1") {
                return true;
            }

            if (value == "false" || value == "0") {
                return false;
            }

            throw std::invalid_argument("Mach error: failed to convert string to bool");
        }
        else {
            static_assert(
                std::same_as<T, void>,
                "Mach error: fromString only supports primitive types"
                );
        }
    }
}
