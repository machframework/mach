#pragma once

#include <string>
#include <unordered_map>

#include <mach/detail/routing/Endpoint.hpp>
#include "RoutingStatus.hpp"

namespace mach::detail::routing
{
	struct RouteMatch {
		routing::RoutingStatus status = RoutingStatus::Found;
		routing::Endpoint* endpoint = nullptr;
		std::unordered_map<std::string, std::string> params;

		explicit RouteMatch(routing::RoutingStatus status)
			: status(status)
		{ }

		explicit RouteMatch(routing::Endpoint* endpoint)
			: status(routing::RoutingStatus::Found),
			endpoint(endpoint)
		{ }

		RouteMatch(
			routing::Endpoint* endpoint, std::unordered_map<std::string, std::string>&& params
		) : endpoint(endpoint), 
			params(std::move(params))
		{ }
	};	
}