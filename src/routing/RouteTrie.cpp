#include "RouteTrie.hpp"

#include <algorithm>
#include <format>
#include <functional>
#include <iostream>
#include <stdexcept>

namespace
{
	bool isParameter(std::string_view segment) {
		return segment.front() == '{'
			&& segment.back() == '}';
	}

	std::string extractParameter(std::string_view segment) {
		segment.remove_prefix(1);
		segment.remove_suffix(1);

		return std::string(segment);
	}
}

namespace mach::detail::routing
{
	void RouteTrie::addRoute(
		std::vector<std::string_view>&& segments,
		routing::Endpoint* endpoint
	) 
	{
		RouteNode* curr = &m_root;

		for (auto it = segments.begin(); it != segments.end(); ++it) {
			const auto& nextSegmentKey = *it;
			
			auto nextSegment = curr->childrenByStaticSegment.find(std::string(nextSegmentKey));

			// child does not exist yet
			if (nextSegment == curr->childrenByStaticSegment.end()) {
				RouteNode* next = nullptr;
				
				if (isParameter(nextSegmentKey)) {
					if (!curr->parameterizedChild) {
						curr->parameterizedChild = std::make_unique<RouteNode>(extractParameter(nextSegmentKey));
					}

					next = curr->parameterizedChild.get();
				}
				else {
					auto [pos, inserted] = curr->childrenByStaticSegment.emplace(
						nextSegmentKey,
						std::make_unique<RouteNode>(nextSegmentKey)
					);

					next = pos->second.get();
				}
				
				curr = next;
			}
			else {
				curr = nextSegment->second.get();
			}
		}

		// same route and method, reject
		if (curr->endpointsByMethod.contains(endpoint->method)) {
			throw std::logic_error(
				std::format(
					"Duplicate route registered: {} {}",
					mach::http::toString(endpoint->method),
					segmentsToPath(segments)
				)
			);
		}

		// same route, different method
		curr->endpointsByMethod.emplace(endpoint->method, endpoint);
	}

	routing::RouteMatch RouteTrie::matchRoute(
		mach::http::Method method,
		std::vector<std::string_view>&& segments
	) const
	{
		const RouteNode* curr = &m_root;
		std::unordered_map<std::string, std::string> params;

		for (auto it = segments.begin(); it != segments.end(); ++it) {
			if (!curr) {
				return routing::RouteMatch(RoutingStatus::NotFound);
			}

			auto nextSegment = curr->childrenByStaticSegment.find(std::string(*it));
			if (nextSegment == curr->childrenByStaticSegment.end()) {
				// check for parameters
				if (curr->parameterizedChild) {
					const auto childNode = curr->parameterizedChild.get();
					params.emplace(childNode->segmentKey, *it);

					if (std::next(it) == segments.end()) {
						if (childNode->endpointsByMethod.empty()) {
							return routing::RouteMatch(RoutingStatus::NotFound);
						}
						if (!childNode->endpointsByMethod.contains(method)) {
							return routing::RouteMatch(RoutingStatus::MethodNotAllowed);
						}

						return routing::RouteMatch(childNode->endpointsByMethod.find(method)->second, std::move(params));
					}

					curr = curr->parameterizedChild.get();
					continue;
				}

				return routing::RouteMatch(RoutingStatus::NotFound);
			}

			if (std::next(it) == segments.end()) {
				const auto& endpointsByMethod = nextSegment->second->endpointsByMethod;

				if (endpointsByMethod.contains(method)) {
					return routing::RouteMatch(endpointsByMethod.find(method)->second, std::move(params));
				}
				
				return routing::RouteMatch(RoutingStatus::MethodNotAllowed);
			}

			curr = nextSegment->second.get();
		}

		// reached the end of the segment list without finding a match
		return routing::RouteMatch(routing::RoutingStatus::NotFound);
	}

	void RouteTrie::debugDump() const {
		std::function<void(const RouteNode&, const std::string&, bool, bool)> print =
			[&](const RouteNode& node, const std::string& prefix, bool isLast, bool isParam) {

			const std::string connector = isLast ? "\\-- " : "|-- ";

			// Build label
			std::string label = node.segmentKey.empty()
				? "[root]"
				: (isParam ? "{" + node.segmentKey + "}" : node.segmentKey);

			if (!node.endpointsByMethod.empty()) {
				label += " [";
				bool first = true;
				for (const auto& [method, _] : node.endpointsByMethod) {
					if (!first) label += ", ";
					label += toString(method);
					first = false;
				}
				label += "]";
			}

			std::cout << prefix << connector << label << "\n";

			const std::string childPrefix = prefix + (isLast ? "    " : "|   ");
			const bool hasParamChild = node.parameterizedChild != nullptr;

			// Collect and sort static children
			std::vector<std::string> keys;
			keys.reserve(node.childrenByStaticSegment.size());
			for (const auto& [key, _] : node.childrenByStaticSegment)
				keys.push_back(key);
			std::sort(keys.begin(), keys.end());

			for (size_t i = 0; i < keys.size(); ++i) {
				const bool lastChild = !hasParamChild && (i == keys.size() - 1);
				print(*node.childrenByStaticSegment.at(keys[i]), childPrefix, lastChild, false);
			}

			if (hasParamChild)
				print(*node.parameterizedChild, childPrefix, true, true);
			};

		print(m_root, "", true, false);
		std::cout << '\n';
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
