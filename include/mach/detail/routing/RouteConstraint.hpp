#pragma once

#include <optional>
#include <string_view>

namespace mach::detail::routing
{
	enum class RouteConstraint {
		Int,
		String
	};

	std::string_view toString(RouteConstraint constraint);
	std::optional<RouteConstraint> toRouteConstraint(std::string_view value);
	bool satisfiesConstraint(std::string_view value, RouteConstraint constraint);
}
