#pragma once

#include <string>
#include <unordered_map>

#include "routing/Endpoint.hpp"

namespace mach::detail::application
{
	struct ExecutionPlan {
		// middleware[]
		routing::Endpoint* endpoint;
	};
}
