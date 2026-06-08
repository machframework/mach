#pragma once

#include <memory>
#include <string>
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
			std::vector<std::string>&& segments,
			mach::http::Method method,
			routing::Endpoint* endpoint
		);

		routing::RouteMatch matchRoute(
			mach::http::Method method,
			const std::vector<std::string>& segments
		);

		void debugDump() const;

	private:
		struct RouteNode {
			std::string segmentKey;
			std::unordered_map<mach::http::Method, routing::Endpoint*> endpointsByMethod;
			std::unordered_map<std::string, std::unique_ptr<RouteNode>> childrenBySegment;

			RouteNode(std::string segmentKey = "")
				: segmentKey(std::move(segmentKey))
			{ }
		};

		RouteNode m_root;
	};
}