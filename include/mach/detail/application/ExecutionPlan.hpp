#pragma once

#include <string>
#include <unordered_map>

#include <mach/detail/routing/RouteEndpoint.hpp>
#include <mach/detail/routing/RoutingStatus.hpp>

namespace mach::detail::application
{
	struct ExecutionPlan {
		routing::RoutingStatus status = routing::RoutingStatus::NotFound;
		routing::RouteEndpoint* endpoint = nullptr;
		std::unordered_map<std::string, std::string> params;

		bool found() const noexcept {
			return status == routing::RoutingStatus::Found;
		}
	};
}
