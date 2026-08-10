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
        RoutingStatus status = RoutingStatus::Found;
        RouteEndpoint* endpoint = nullptr;
        std::unordered_map<std::string, std::string> params;
        std::unordered_set<mach::http::Method> allowedMethods;

        explicit RouteMatch(RoutingStatus status) : status(status) {}

        explicit RouteMatch(RouteEndpoint* endpoint)
            : endpoint(endpoint) {}

        explicit RouteMatch(std::unordered_set<mach::http::Method> allowedMethods)
            : status(RoutingStatus::MethodNotAllowed),
              allowedMethods(std::move(allowedMethods)) {}

        RouteMatch(
            RouteEndpoint* endpoint,
            std::unordered_map<std::string, std::string>&& params)
            : endpoint(endpoint), params(std::move(params)) {}
    };
}