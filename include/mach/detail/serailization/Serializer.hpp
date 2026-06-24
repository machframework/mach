#pragma once

#include <concepts>
#include <string>

namespace mach::detail::serialization
{
	class Serializer {

	public:
		template <typename T>
		static std::string serialize(const T& value);
	};
	
	template <typename T>
	std::string Serializer::serialize(const T& value) {
		if constexpr (std::same_as<T, std::string>) {
			return value;
		}
		else if constexpr (
			std::integral<T> ||
			std::floating_point<T>
			) {
			return std::to_string(value);
		}
		else {
			static_assert(
				std::same_as<T, void>,
				"Mach error: unsupported response type"
				);
		}
	}
}
