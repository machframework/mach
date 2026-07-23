#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include <mach/http/Method.hpp>

#include <mach/detail/routing/RouteEndpoint.hpp>
#include <mach/detail/routing/RoutingStatus.hpp>

namespace mach::detail::routing
{
    struct RouteMatch {
        routing::RoutingStatus status = RoutingStatus::Found;
        routing::RouteEndpoint* endpoint = nullptr;
        std::unordered_map<std::string, std::string> params;
        std::unordered_set<mach::http::Method> allowedMethods;

        explicit RouteMatch(routing::RoutingStatus status) : status(status) {}

        explicit RouteMatch(routing::RouteEndpoint* endpoint)
            : status(routing::RoutingStatus::Found), endpoint(endpoint) {}

        explicit RouteMatch(std::unordered_set<mach::http::Method> allowedMethods)
            : status(routing::RoutingStatus::MethodNotAllowed),
              allowedMethods(std::move(allowedMethods)) {}

        RouteMatch(
            routing::RouteEndpoint* endpoint,
            std::unordered_map<std::string, std::string>&& params)
            : endpoint(endpoint), params(std::move(params)) {}
    };
}