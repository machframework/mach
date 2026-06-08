#include "RouteTrie.hpp"

#include <algorithm>
#include <format>
#include <functional>
#include <iostream>
#include <stdexcept>

namespace mach::detail::routing
{
	void RouteTrie::addRoute(
		std::vector<std::string>&& segments,
		mach::http::Method method,
		routing::Endpoint* endpoint
	) 
	{
		RouteNode* curr = &m_root;

		for (auto it = segments.begin(); it != segments.end(); ++it) {
			const auto& currSegmentKey = *it;
			
			auto nextSegment = curr->childrenBySegment.find(currSegmentKey);

			// child does not exist yet
			if (nextSegment == curr->childrenBySegment.end()) {
				auto newNode = std::make_unique<RouteNode>(currSegmentKey);

				if (std::next(it) == segments.end()) {
					newNode->endpointsByMethod.emplace(method, endpoint);
					curr->childrenBySegment.emplace(currSegmentKey, std::move(newNode));
					
					return;
				}

				auto [pos, inserted] = curr->childrenBySegment.emplace(
					currSegmentKey,
					std::make_unique<RouteNode>(currSegmentKey)
				);

				// move to child
				curr = pos->second.get();
			}
			else {
				if (std::next(it) == segments.end()) {
					if (nextSegment->second->endpointsByMethod.contains(method)) {
						throw std::logic_error(
							std::format(
								"Duplicate route registered: {} {}",
								mach::http::toString(method),
								currSegmentKey
							)
						);
					}

					// same route, different method
					else {
						nextSegment->second->endpointsByMethod.emplace(
							method,
							endpoint
						);

						return;
					}
				}

				curr = nextSegment->second.get();
			}
		}
	}

	routing::RouteMatch RouteTrie::matchRoute(
		mach::http::Method method,
		const std::vector<std::string>& segments
	) 
	{
		RouteNode* curr = &m_root;

		for (auto it = segments.begin(); it != segments.end(); ++it) {
			if (!curr) {
				return routing::RouteMatch(RouteMatchStatus::NotFound);
			}

			auto nextSegment = curr->childrenBySegment.find(*it);
			if (nextSegment == curr->childrenBySegment.end()) {
				return routing::RouteMatch(RouteMatchStatus::NotFound);
			}

			if (std::next(it) == segments.end()) {
				if (nextSegment->second->endpointsByMethod.size() == 0) {
					return routing::RouteMatch(RouteMatchStatus::NotFound);
				}

				if (nextSegment->second->endpointsByMethod.contains(method)) {
					return routing::RouteMatch(nextSegment->second->endpointsByMethod.find(method)->second);
				}
				
				return routing::RouteMatch(RouteMatchStatus::MethodNotAllowed);
			}

			curr = nextSegment->second.get();
		}

		// reached the end of the segment list without finding a match
		return routing::RouteMatch(routing::RouteMatchStatus::NotFound);
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
				keys.reserve(node.childrenBySegment.size());
				for (const auto& [key, _] : node.childrenBySegment)
					keys.push_back(key);
				std::sort(keys.begin(), keys.end());

				for (size_t i = 0; i < keys.size(); ++i) {
					const auto& child = *node.childrenBySegment.at(keys[i]);
					print(child, childPrefix, i == keys.size() - 1);
				}
			};

		print(m_root, "", true);
		std::cout << std::endl;
	}
}
