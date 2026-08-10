#pragma once

#include <deque>
#include <string>
#include <unordered_map>

#include <mach/Request.hpp>

#include <mach/detail/application/ExecutionPlan.hpp>
#include <mach/detail/routing/RouteEndpoint.hpp>
#include <mach/detail/routing/RouteMatch.hpp>
#include <mach/detail/routing/RouteTrie.hpp>

namespace mach::detail::routing
{
    class Router {

    public:
        [[nodiscard]] application::ExecutionPlan route(mach::Request& request) const;
        void mapRoute(RouteEndpoint&& endpoint);

    private:
        routing::RouteMatch matchRoute(mach::Request& request) const;
        static std::unordered_map<std::string, std::string> extractQuery(std::string& target);

        std::deque<routing::RouteEndpoint> m_endpoints;
        routing::RouteTrie m_routes;
    };
}
