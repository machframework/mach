#pragma once

#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <mach/http/Method.hpp>

#include "RouteConstraint.hpp"
#include "routing/RouteMatch.hpp"

namespace mach::detail::routing
{
	class RouteTrie {

	public:
		RouteTrie() = default;

		void addRoute(
			std::vector<std::string_view>&& segments,
			routing::RouteEndpoint* endpoint
		);

		routing::RouteMatch matchRoute(
			mach::http::Method method,
			std::vector<std::string_view>&& segments
		) const;

		void debugDump() const;

	private:
		struct RouteNode {
			std::string segmentKey;
			std::unordered_map<mach::http::Method, routing::RouteEndpoint*> endpointsByMethod;

			std::unordered_map<std::string, std::unique_ptr<RouteNode>> childrenByStaticSegment;
			std::unordered_map<std::optional<routing::RouteConstraint>, std::unique_ptr<RouteNode>> constrainedParameterChildren;

			explicit RouteNode(std::string_view segmentKey = "")
				: segmentKey(segmentKey)
			{ }

			RouteNode(const RouteNode&) = delete;
			RouteNode& operator=(const RouteNode&) = delete;

			RouteNode(RouteNode&&) noexcept = default;
			RouteNode& operator=(RouteNode&&) noexcept = default;
		};

		static std::string segmentsToPath(const std::vector<std::string_view>& segments);

		RouteNode m_root;
	};
}
