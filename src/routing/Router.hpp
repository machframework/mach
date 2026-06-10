#pragma once

#include <deque>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <mach/Request.hpp>

#include "application/ExecutionPlan.hpp"
#include "Endpoint.hpp"
#include "RouteMatch.hpp"
#include "RouteTrie.hpp"

namespace mach::detail::routing
{
	class Router {

	public:
		application::ExecutionPlan route(const mach::Request& request) const;
		void addRoute(Endpoint&& route);

	private:
		routing::RouteMatch matchRoute(const mach::Request& request) const;

		std::deque<routing::Endpoint> m_endpoints;
		routing::RouteTrie m_routes;
	};
}
