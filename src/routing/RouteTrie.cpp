#include "RouteTrie.hpp"

#include <algorithm>
#include <format>
#include <functional>
#include <iostream>
#include <optional>
#include <utility>
#include <stdexcept>

#include "RouteConstraint.hpp"

namespace
{
	using mach::detail::routing::RouteConstraint;

	bool isParameter(std::string_view segment) {
		return segment.front() == '{'
			&& segment.back() == '}';
	}

	std::pair<std::string, RouteConstraint> extractParameter(std::string_view segment) {
		segment.remove_prefix(1);
		segment.remove_suffix(1);

		auto pos = segment.find(':');
		if (pos == std::string_view::npos) {
			return {
				std::string(segment),
				RouteConstraint::String
			};
		}

		auto param = segment.substr(0, pos);
		auto constraint = segment.substr(pos + 1);

		// find constraint
		auto constraintType = mach::detail::routing::toRouteConstraint(constraint);
		if (!constraintType) {
			throw std::invalid_argument("Invalid constraint type");
		}

		if (param.empty()) {
			throw std::invalid_argument("Parameter name cannot be empty");
		}
	

		return {
			std::string(param),
			*constraintType
		};
	}

	std::unordered_map<std::string, std::string> makeRouteParameters(const std::vector<std::string>& names, std::vector<std::string>&& values) {
		std::unordered_map<std::string, std::string> params;

		if (names.size() != values.size()) {
			throw std::runtime_error("Invalid route matched");
		}

		params.reserve(names.size());

		for (size_t i = 0; i < names.size(); i++) {
			params.emplace(names[i], values[i]);
		}

		return params;
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

		// registering root 
		if (segments.empty()) {
			curr->endpointsByMethod.emplace(endpoint->method, endpoint);
			return;
		}

		for (auto it = segments.begin(); it != segments.end(); ++it) {
			const auto& nextSegmentKey = *it;
			
			auto nextSegment = curr->childrenByStaticSegment.find(std::string(nextSegmentKey));

			// child does not exist yet
			if (nextSegment == curr->childrenByStaticSegment.end()) {
				RouteNode* next = nullptr;
				
				if (isParameter(nextSegmentKey)) {
					// find constraints
					const auto [parameter, constraint] = extractParameter(nextSegmentKey);

					if (!curr->constrainedParameterChildren.contains(constraint)) {
						curr->constrainedParameterChildren.emplace(
							constraint,
							std::make_unique<RouteNode>(parameter)
						);
					}

					//add constrained parameter to parameter list
					endpoint->parameterNames.push_back(parameter);

					next = curr->constrainedParameterChildren.find(constraint)->second.get();
				}
				else {
					auto [pos, inserted] = curr->childrenByStaticSegment.emplace(
						std::string(nextSegmentKey),
						std::make_unique<RouteNode>(std::string(nextSegmentKey))
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

		//debugDump();
	}

	routing::RouteMatch RouteTrie::matchRoute(
		mach::http::Method method,
		std::vector<std::string_view>&& segments
	) const
	{
		const RouteNode* curr = &m_root;

		std::vector<std::string> capturedValues;

		// check for root
		if (segments.empty()) {
			if (curr->endpointsByMethod.contains(method)) {
				auto endpoint = curr->endpointsByMethod.find(method)->second;
				return RouteMatch(endpoint);
			}
		}

		for (auto it = segments.begin(); it != segments.end(); ++it) {
			if (!curr) {
				return routing::RouteMatch(RoutingStatus::NotFound);
			}

			auto nextSegment = curr->childrenByStaticSegment.find(std::string(*it));
			if (nextSegment == curr->childrenByStaticSegment.end()) {
				// check for parameters
				if (!curr->constrainedParameterChildren.empty()) {
					// find the parameter type of the segment
					RouteConstraint constraint = RouteConstraint::String; // default to string

					if (satisfiesConstraint(*it, RouteConstraint::Int)) {
						constraint = RouteConstraint::Int;
					}

					RouteNode* childNode = nullptr;

					if (curr->constrainedParameterChildren.contains(constraint)) {
						childNode = curr->constrainedParameterChildren.find(constraint)->second.get();
					}
					else if (curr->constrainedParameterChildren.contains(RouteConstraint::String)) {
						childNode = curr->constrainedParameterChildren.find(RouteConstraint::String)->second.get();
					}
					else {
						// not found for the given param types
						return RouteMatch(RoutingStatus::NotFound);
					}

					capturedValues.push_back(std::string(*it));

					if (std::next(it) == segments.end()) {
						if (childNode->endpointsByMethod.empty()) {
							return routing::RouteMatch(RoutingStatus::NotFound);
						}
						if (!childNode->endpointsByMethod.contains(method)) {
							return routing::RouteMatch(RoutingStatus::MethodNotAllowed);
						}

						auto endpoint = childNode->endpointsByMethod.find(method)->second;

						return routing::RouteMatch(
							endpoint,
							std::move(
								makeRouteParameters(endpoint->parameterNames, std::move(capturedValues))
							)
						);
					}

					curr = childNode;
					continue;
				}

				return routing::RouteMatch(RoutingStatus::NotFound);
			}

			if (std::next(it) == segments.end()) {
				const auto& endpointsByMethod = nextSegment->second->endpointsByMethod;

				if (endpointsByMethod.contains(method)) {
					auto endpoint = endpointsByMethod.find(method)->second;

					return routing::RouteMatch(
						endpoint,
						std::move(
							makeRouteParameters(endpoint->parameterNames, std::move(capturedValues))
						)
					);
				}
				
				return routing::RouteMatch(RoutingStatus::MethodNotAllowed);
			}

			curr = nextSegment->second.get();
		}

		// reached the end of the segment list without finding a match
		return routing::RouteMatch(routing::RoutingStatus::NotFound);
	}

	void RouteTrie::debugDump() const {
		std::function<void(const RouteNode&, const std::string&, bool, bool, const std::optional<routing::RouteConstraint>&)> print =
			[&](const RouteNode& node, const std::string& prefix, bool isLast, bool isParam, const std::optional<routing::RouteConstraint>& constraint) {
			const std::string connector = isLast ? "\\-- " : "|-- ";
			// Build label
			std::string label = node.segmentKey.empty()
				? "[root]"
				: (isParam
					? "{" + node.segmentKey + (constraint ? ":" + std::string(toString(*constraint)) : "") + "}"
					: node.segmentKey);
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
			const bool hasParamChildren = !node.constrainedParameterChildren.empty();
			// Collect and sort static children
			std::vector<std::string> keys;
			keys.reserve(node.childrenByStaticSegment.size());
			for (const auto& [key, _] : node.childrenByStaticSegment)
				keys.push_back(key);
			std::sort(keys.begin(), keys.end());
			for (size_t i = 0; i < keys.size(); ++i) {
				const bool lastChild = !hasParamChildren && (i == keys.size() - 1);
				print(*node.childrenByStaticSegment.at(keys[i]), childPrefix, lastChild, false, std::nullopt);
			}
			// Collect and sort parameterized children by constraint name for stable output
			std::vector<std::optional<routing::RouteConstraint>> constraints;
			constraints.reserve(node.constrainedParameterChildren.size());
			for (const auto& [constraintKey, _] : node.constrainedParameterChildren)
				constraints.push_back(constraintKey);

			std::sort(constraints.begin(), constraints.end(),
				[](const auto& a, const auto& b) {
					const std::string_view sa = a ? toString(*a) : "";
					const std::string_view sb = b ? toString(*b) : "";
					return sa < sb;
				});

			for (size_t i = 0; i < constraints.size(); ++i) {
				const bool lastChild = (i == constraints.size() - 1);
				print(*node.constrainedParameterChildren.at(constraints[i]), childPrefix, lastChild, true, constraints[i]);
			}
		};

		print(m_root, "", true, false, std::nullopt);
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
