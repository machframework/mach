#pragma once

#include <string>
#include <unordered_map>

#include "routing/Endpoint.hpp"
#include "routing/RouteMatchStatus.hpp"

namespace mach::detail::application
{
	struct ExecutionPlan {
		routing::RouteMatchStatus status;
		// middleware[]
		routing::Endpoint* endpoint;
	};
}
