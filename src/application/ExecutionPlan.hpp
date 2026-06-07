#pragma once

#include "routing/Endpoint.hpp"

namespace mach::detail::application
{
	struct ExecutionPlan {
		// middleware[]
		mach::detail::routing::Endpoint* endpoint;
	};
}
