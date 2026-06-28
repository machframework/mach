#pragma once

#include <string_view>

#include <mach/Json.hpp>

namespace mach::detail::binding
{
	class BodyBinder {

	public:
		template <typename T>
		T bind(std::string_view body);
	};

	template <typename T>
	T BodyBinder::bind(std::string_view body) {
		auto json = mach::Json::parse(body);
		return json.get<T>();
	}
}
