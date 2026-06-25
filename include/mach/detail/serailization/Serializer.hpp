#pragma once

#include <concepts>
#include <string>

#include <mach/Json.hpp>

namespace mach::detail::serialization
{
	class Serializer {

	public:
		template <typename T>
		static std::string serialize(const T& value);
	};
	
    template <typename T>
    std::string Serializer::serialize(const T& value)
    {
        if constexpr (std::same_as<std::remove_cvref_t<T>, std::string>) {
            return value;
        }
        else if constexpr (std::same_as<std::remove_cvref_t<T>, mach::Json>) {
            return value.dump();
        }
        else if constexpr (
            std::integral<std::remove_cvref_t<T>> ||
            std::floating_point<std::remove_cvref_t<T>>
            ) {
            return std::to_string(value);
        }
        else {
            return mach::Json(value).dump();
        }
    }
}
