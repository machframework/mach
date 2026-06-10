#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <mach/http/Method.hpp>

#include "routing/RouteMatch.hpp"

namespace mach::detail::routing
{
	class RouteTrie {

	public:
		RouteTrie() = default;

		void addRoute(
			std::vector<std::string_view>&& segments,
			routing::Endpoint* endpoint
		);

		routing::RouteMatch matchRoute(
			mach::http::Method method,
			std::vector<std::string_view>&& segments
		) const;

		void debugDump() const;

	private:
		struct RouteNode {
			std::string segmentKey;
			std::unordered_map<mach::http::Method, routing::Endpoint*> endpointsByMethod;

			std::unordered_map<std::string, std::unique_ptr<RouteNode>> childrenByStaticSegment;
			std::unique_ptr<RouteNode> parameterizedChild;

			explicit RouteNode(std::string_view segmentKey = "")
				: segmentKey(std::move(segmentKey))
			{ }
		};

		static std::string segmentsToPath(const std::vector<std::string_view>& segments);

		RouteNode m_root;
	};
}
