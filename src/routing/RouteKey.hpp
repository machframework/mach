#pragma once

#include <string>

#include <mach/http/Method.hpp>

namespace mach::detail::routing
{
	struct RouteKey {
		mach::http::Method method;
		std::string pattern;

		bool operator==(const RouteKey&) const = default;
	};
}