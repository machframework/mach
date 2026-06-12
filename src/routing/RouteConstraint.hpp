#pragma once

#include <optional>
#include <string_view>

namespace mach::detail::routing
{
	enum class RouteConstraint {
		Int
	};

	std::optional<RouteConstraint> toRouteConstraint(std::string_view value);
	bool satisfiesConstraint(std::string_view value, RouteConstraint constraint);
}
