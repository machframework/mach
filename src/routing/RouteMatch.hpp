#pragma once

#include "routing/Endpoint.hpp"

namespace mach::detail::routing
{
	enum class RouteMatchStatus {
		Found,
		NotFound,
		MethodNotAllowed
	};

	struct RouteMatch {
		RouteMatchStatus status;
		routing::Endpoint* endpoint = nullptr;

		RouteMatch(RouteMatchStatus status)
			: status(status)
		{ }

		RouteMatch(routing::Endpoint* endpoint)
			: status(RouteMatchStatus::Found),
			endpoint(endpoint)
		{ }
	};	
}