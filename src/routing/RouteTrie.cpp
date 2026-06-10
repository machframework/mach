#include "RouteTrie.hpp"

#include <algorithm>
#include <format>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string_view>

namespace mach::detail::routing
{
	void RouteTrie::addRoute(
		std::vector<std::string_view>&& segments,
		routing::Endpoint* endpoint
	) 
	{
		RouteNode* curr = &m_root;

		for (auto it = segments.begin(); it != segments.end(); ++it) {
			const auto& currSegmentKey = *it;
			
			auto nextSegment = curr->childrenByStaticSegment.find(std::string(currSegmentKey));

			// child does not exist yet
			if (nextSegment == curr->childrenByStaticSegment.end()) {
				auto [pos, inserted] = curr->childrenByStaticSegment.emplace(
					currSegmentKey,
					std::make_unique<RouteNode>(currSegmentKey)
				);

				curr = pos->second.get();
			}
			else {
				curr = nextSegment->second.get();
			}
		}

		if (curr->endpointsByMethod.contains(endpoint->method)) {
			throw std::logic_error(
				std::format(
					"Duplicate route registered: {} {}",
					mach::http::toString(endpoint->method),
					segmentsToPath(segments)
				)
			);
		}

		curr->endpointsByMethod.emplace(endpoint->method, endpoint);
	}

	routing::RouteMatch RouteTrie::matchRoute(
		mach::http::Method method,
		std::vector<std::string_view>&& segments
	) const
	{
		const RouteNode* curr = &m_root;

		for (auto it = segments.begin(); it != segments.end(); ++it) {
			if (!curr) {
				return routing::RouteMatch(RoutingStatus::NotFound);
			}

			auto nextSegment = curr->childrenByStaticSegment.find(std::string(*it));
			if (nextSegment == curr->childrenByStaticSegment.end()) {
				// check for parameters
				     
				return routing::RouteMatch(RoutingStatus::NotFound);
			}

			if (std::next(it) == segments.end()) {
				if (nextSegment->second->endpointsByMethod.size() == 0) {
					return routing::RouteMatch(RoutingStatus::NotFound);
				}

				if (nextSegment->second->endpointsByMethod.contains(method)) {
					return routing::RouteMatch(nextSegment->second->endpointsByMethod.find(method)->second);
				}
				
				return routing::RouteMatch(RoutingStatus::MethodNotAllowed);
			}

			curr = nextSegment->second.get();
		}

		// reached the end of the segment list without finding a match
		return routing::RouteMatch(routing::RoutingStatus::NotFound);
	}

	void RouteTrie::debugDump() const {
		std::function<void(const RouteNode&, const std::string&, bool)> print =
			[&](const RouteNode& node, const std::string& prefix, bool isLast) {
				// Print current node
				std::string connector = isLast ? "\\-- " : "|-- ";
				std::string label = node.segmentKey.empty() ? "[root]" : node.segmentKey;

				// Collect methods if any endpoints are registered
				if (!node.endpointsByMethod.empty()) {
					std::string methods = " [";
					bool first = true;
					for (const auto& [method, _] : node.endpointsByMethod) {
						if (!first) methods += ", ";
						methods += toString(method); // adjust to your actual method→string utility
						first = false;
					}
					methods += "]";
					label += methods;
				}

				std::cout << prefix << connector << label << "\n";

				// Prepare prefix for children
				std::string childPrefix = prefix + (isLast ? "    " : "|   ");

				// Collect and sort children keys for stable output
				std::vector<std::string> keys;
				keys.reserve(node.childrenByStaticSegment.size());
				for (const auto& [key, _] : node.childrenByStaticSegment)
					keys.push_back(key);
				std::sort(keys.begin(), keys.end());

				for (size_t i = 0; i < keys.size(); ++i) {
					const auto& child = *node.childrenByStaticSegment.at(keys[i]);
					print(child, childPrefix, i == keys.size() - 1);
				}
			};

		print(m_root, "", true);
		std::cout << std::endl;
	}

	std::string RouteTrie::segmentsToPath(const std::vector<std::string_view>& segments) {
		std::string path = "/";

		for (std::size_t i = 0; i < segments.size(); ++i) {
			path += segments[i];

			if (i + 1 < segments.size()) {
				path += '/';
			}
		}

		return path;
	}
}
