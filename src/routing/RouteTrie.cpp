#include "RouteTrie.hpp"

#include <format>
#include <stdexcept>

namespace mach::detail::routing
{
	void RouteTrie::addRoute(
		std::vector<std::string>&& segments,
		mach::http::Method method,
		routing::Endpoint* endpoint
	) 
	{
		RouteNode* curr = &root;

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
					};
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
		RouteNode* curr = &root;

		for (auto it = segments.begin(); it != segments.end(); ++it) {
			if (!curr) {
				return routing::RouteMatch(RouteMatchStatus::NotFound);
			}

			if (std::next(it) == segments.end()) {
				if (curr->endpointsByMethod.contains(method)) {
					return routing::RouteMatch(curr->endpointsByMethod.find(method)->second);
				}

				return routing::RouteMatch(RouteMatchStatus::MethodNotAllowed);
			}

			auto nextSegment = curr->childrenBySegment.find(*it);
			if (nextSegment == curr->childrenBySegment.end()) {
				return routing::RouteMatch(RouteMatchStatus::NotFound);
			}

			curr = nextSegment->second.get();
		}
	}
}
