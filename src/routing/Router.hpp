#pragma once

#include <string_view>
#include <unordered_map>

#include <mach/Request.hpp>

#include "application/ExecutionPlan.hpp"
#include "Endpoint.hpp"
#include "RouteKey.hpp"

namespace mach::detail::routing
{
	class Router {

	public:
		application::ExecutionPlan route(const mach::Request& request) const;
		void addRoute(Endpoint&& route);

	private:
		Endpoint* matchRoute(const mach::Request& request) const;
	};
}
