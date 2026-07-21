#pragma once

#include <string_view>

#include <mach/Json.hpp>

#include <mach/detail/exceptions/BodyBindingException.hpp>

namespace mach::detail::binding
{
	class BodyBinder {

	public:
		template <typename T>
		T bind(std::string_view body);
	};

	template <typename T>
	T BodyBinder::bind(std::string_view body) {
		if (body.empty()) {
			throw exceptions::BodyBindingException(
				"The request body is required."
			);
		}

		try {
			auto json = mach::Json::parse(body);
			return json.get<T>();
		}
		catch (const nlohmann::json::parse_error&) {
			throw exceptions::BodyBindingException(
				"The request body contains invalid JSON."
			);
		}
		catch (const nlohmann::json::exception&) {
			throw exceptions::BodyBindingException(
				"The request body could not be bound to the requested type."
			);
		}
	}
}
