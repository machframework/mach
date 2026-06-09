#pragma once

#include "Endpoint.hpp"
#include "RouteMatchStatus.hpp"

namespace mach::detail::routing
{
	struct RouteMatch {
		routing::RouteMatchStatus status;
		routing::Endpoint* endpoint = nullptr;

		explicit RouteMatch(routing::RouteMatchStatus status)
			: status(status)
		{ }

		explicit RouteMatch(routing::Endpoint* endpoint)
			: status(routing::RouteMatchStatus::Found),
			endpoint(endpoint)
		{ }
	};	
}