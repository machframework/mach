#pragma once

#include "Endpoint.hpp"
#include "RoutingStatus.hpp"

namespace mach::detail::routing
{
	struct RouteMatch {
		routing::RoutingStatus status;
		routing::Endpoint* endpoint = nullptr;

		explicit RouteMatch(routing::RoutingStatus status)
			: status(status)
		{ }

		explicit RouteMatch(routing::Endpoint* endpoint)
			: status(routing::RoutingStatus::Found),
			endpoint(endpoint)
		{ }
	};	
}