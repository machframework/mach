#pragma once

#include <string>
#include <unordered_map>

#include "routing/Endpoint.hpp"
#include "routing/RoutingStatus.hpp"

namespace mach::detail::application
{
	struct ExecutionPlan {
		routing::RoutingStatus status;
		// middleware[]
		routing::Endpoint* endpoint;

		bool found() const noexcept {
			return status == routing::RoutingStatus::Found;
		}
	};
}
