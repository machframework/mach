#pragma once

#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <mach/detail/routing/RouteConstraint.hpp>
#include <mach/detail/routing/RouteMatch.hpp>
#include <mach/http/Method.hpp>

namespace mach::detail::routing
{
    class RouteTrie {

    public:
        RouteTrie() = default;

        void mapRoute(std::vector<std::string_view>&& segments, RouteEndpoint* endpoint);

        [[nodiscard]] RouteMatch matchRoute(
            mach::http::Method method,
            std::vector<std::string_view>&& segments) const;

        void debugDump() const;

    private:
        struct RouteNode {
            std::string segmentKey;
            std::unordered_map<mach::http::Method, RouteEndpoint*> endpointsByMethod;

            std::unordered_map<std::string, std::unique_ptr<RouteNode>> childrenByStaticSegment;
            std::unordered_map<std::optional<RouteConstraint>, std::unique_ptr<RouteNode>>
                constrainedParameterChildren;

            explicit RouteNode(std::string_view segmentKey = "") : segmentKey(segmentKey) {}

            RouteNode(const RouteNode&) = delete;
            RouteNode& operator=(const RouteNode&) = delete;

            RouteNode(RouteNode&&) noexcept = default;
            RouteNode& operator=(RouteNode&&) noexcept = default;
        };

        RouteMatch matchRoute(
            mach::http::Method method,
            const std::vector<std::string_view>& segments,
            std::size_t index,
            std::vector<std::string>& capturedValues,
            std::unordered_set<mach::http::Method>& allowedMethods,
            const RouteNode* curr) const;

        RouteNode m_root;
    };
}
