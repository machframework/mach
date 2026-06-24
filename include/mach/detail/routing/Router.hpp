#pragma once

#include <deque>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <mach/Request.hpp>

#include <mach/detail/application/ExecutionPlan.hpp>
#include <mach/detail/routing/RouteEndpoint.hpp>
#include <mach/detail/routing/RouteMatch.hpp>
#include <mach/detail/routing/RouteTrie.hpp>

namespace mach::detail::routing
{
	class Router {

	public:
		application::ExecutionPlan route(const mach::Request& request) const;
		void addRoute(RouteEndpoint&& endpoint);

	private:
		routing::RouteMatch matchRoute(const mach::Request& request) const;

		std::deque<routing::RouteEndpoint> m_endpoints;
		routing::RouteTrie m_routes;
	};
}
