#pragma once

#include <string>
#include <unordered_map>

#include <mach/detail/routing/RouteEndpoint.hpp>
#include "RoutingStatus.hpp"

namespace mach::detail::routing
{
	struct RouteMatch {
		routing::RoutingStatus status = RoutingStatus::Found;
		routing::RouteEndpoint* endpoint = nullptr;
		std::unordered_map<std::string, std::string> params;

		explicit RouteMatch(routing::RoutingStatus status)
			: status(status)
		{ }

		explicit RouteMatch(routing::RouteEndpoint* endpoint)
			: status(routing::RoutingStatus::Found),
			endpoint(endpoint)
		{ }

		RouteMatch(
			routing::RouteEndpoint* endpoint, std::unordered_map<std::string, std::string>&& params
		) : endpoint(endpoint), 
			params(std::move(params))
		{ }
	};	
}