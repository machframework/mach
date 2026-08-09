#include <mach/detail/routing/RouteTrie.hpp>

#include <algorithm>
#include <format>
#include <functional>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <unordered_set>
#include <utility>

#include <mach/detail/routing/RouteConstraint.hpp>

namespace
{
    using mach::detail::routing::RouteConstraint;

    bool isParameter(std::string_view segment) {
        return segment.front() == '{' && segment.back() == '}';
    }

    std::pair<std::string, RouteConstraint> extractParameter(
        std::string_view pattern,
        std::string_view segment) {
        segment.remove_prefix(1);
        segment.remove_suffix(1);

        auto pos = segment.find(':');
        if (pos == std::string_view::npos) {
            return {std::string(segment), RouteConstraint::String};
        }

        auto param = segment.substr(0, pos);
        auto constraint = segment.substr(pos + 1);

        if (constraint.empty()) {
            throw std::invalid_argument(
                std::format(
                    "Invalid route definition '{}': Route parameter constraint cannot be empty",
                    pattern));
        }

        // find constraint
        auto constraintType = mach::detail::routing::toRouteConstraint(constraint);
        if (!constraintType) {
            throw std::invalid_argument(
                std::format(
                    "Invalid route definition '{}': Unknown route parameter constraint '{}'",
                    pattern,
                    constraint));
        }

        if (param.empty()) {
            return {
                "", // empty param name
                *constraintType};
        }

        return {std::string(param), *constraintType};
    }

    std::unordered_map<std::string, std::string> makeRouteParameters(
        const std::vector<std::string>& names,
        const std::vector<std::string>& values) {
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
    void RouteTrie::mapRoute(
        std::vector<std::string_view>&& segments,
        RouteEndpoint* endpoint) {
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
                    const auto [parameter, constraint] =
                        extractParameter(endpoint->pattern, nextSegmentKey);

                    if (parameter == "") {
                        throw std::invalid_argument(
                            std::format(
                                "Invalid route definition '{}': Route parameter name cannot be "
                                "empty",
                                endpoint->pattern));
                    }

                    if (!curr->constrainedParameterChildren.contains(constraint)) {
                        curr->constrainedParameterChildren.emplace(
                            constraint,
                            std::make_unique<RouteNode>(parameter));
                    }

                    // add constrained parameter to parameter list
                    endpoint->parameterNames.push_back(parameter);

                    next = curr->constrainedParameterChildren.find(constraint)->second.get();
                } else {
                    auto [pos, inserted] = curr->childrenByStaticSegment.emplace(
                        std::string(nextSegmentKey),
                        std::make_unique<RouteNode>(std::string(nextSegmentKey)));

                    next = pos->second.get();
                }

                curr = next;
            } else {
                curr = nextSegment->second.get();
            }
        }

        // same route and method, reject
        if (curr->endpointsByMethod.contains(endpoint->method)) {
            auto conflictingEndpoint = curr->endpointsByMethod.find(endpoint->method)->second;

            if (endpoint->pattern == conflictingEndpoint->pattern) {
                throw std::invalid_argument(
                    std::format(
                        "Invalid route definition '{}': An identical route is already registered "
                        "for method {}",
                        endpoint->pattern,
                        mach::http::toString(endpoint->method)));
            } else {
                throw std::invalid_argument(
                    std::format(
                        "Invalid route definition '{}': The route is ambiguous with existing route "
                        "'{}' for method {}",
                        endpoint->pattern,
                        conflictingEndpoint->pattern,
                        mach::http::toString(endpoint->method)));
            }
        }

        // same route, different method
        curr->endpointsByMethod.emplace(endpoint->method, endpoint);

        // debugDump();
    }

    routing::RouteMatch RouteTrie::matchRoute(
        mach::http::Method method,
        std::vector<std::string_view>&& segments) const {
        const RouteNode* curr = &m_root;

        std::vector<std::string> capturedValues;
        std::unordered_set<mach::http::Method> allowedMethods = {mach::http::Method::Options};

        return matchRoute(method, segments, 0, capturedValues, allowedMethods, curr);
    }

    RouteMatch RouteTrie::matchRoute(
        http::Method method,
        const std::vector<std::string_view>& segments,
        std::size_t index,
        std::vector<std::string>& capturedValues,
        std::unordered_set<http::Method>& allowedMethods,
        const RouteNode* curr) const {
        auto segmentsSize = segments.size();

        // check for root
        if (segments.empty()) {
            if (curr->endpointsByMethod.contains(method)) {
                auto endpoint = curr->endpointsByMethod.find(method)->second;
                return RouteMatch(endpoint);
            }
            if (curr->endpointsByMethod.empty()) {
                return routing::RouteMatch(RoutingStatus::NotFound);
            }

            for (const auto& entry : curr->endpointsByMethod) {
                allowedMethods.insert(entry.first);
            }

            return routing::RouteMatch(allowedMethods);
        }

        // reached the end of the segments without finding a match
        if (index >= segmentsSize || !curr) {
            return RouteMatch(RoutingStatus::NotFound);
        }

        auto segmentKey = std::string(segments[index]);
        auto nextSegment = curr->childrenByStaticSegment.find(segmentKey);

        // try static route first
        if (nextSegment != curr->childrenByStaticSegment.end()) {
            if (index == segments.size() - 1) {
                const auto& endpointsByMethod = nextSegment->second->endpointsByMethod;

                if (endpointsByMethod.contains(method)) {
                    auto endpoint = endpointsByMethod.find(method)->second;

                    return RouteMatch(
                        endpoint,
                        std::move(makeRouteParameters(endpoint->parameterNames, capturedValues)));
                } if (
                    method == http::Method::Head && endpointsByMethod.contains(http::Method::Get)
                ) {
                    auto endpoint = endpointsByMethod.find(http::Method::Get)->second;
                    return routing::RouteMatch(
                        endpoint,
                        std::move(makeRouteParameters(endpoint->parameterNames, capturedValues)));
                }
                if (endpointsByMethod.empty()) {
                    return routing::RouteMatch(RoutingStatus::NotFound);
                }

                for (const auto& entry : endpointsByMethod) {
                    allowedMethods.insert(entry.first);
                }

                return RouteMatch(allowedMethods);
            }

            auto result = matchRoute(
				method,
				segments,
				index + 1,
				capturedValues,
				allowedMethods,
				nextSegment->second.get() // continue down the static route
			);

            if (result.status != RoutingStatus::NotFound) {
                return result;
            }
        }

        // check for parameters
        if (!curr->constrainedParameterChildren.empty()) {
            // find the parameter type of the segment
            RouteConstraint constraint = RouteConstraint::String; // default to string

            if (satisfiesConstraint(segmentKey, RouteConstraint::Int)) {
                constraint = RouteConstraint::Int;
            }

            RouteNode* childNode = nullptr;

            if (curr->constrainedParameterChildren.contains(constraint)) {
                childNode = curr->constrainedParameterChildren.find(constraint)->second.get();
            } else if (curr->constrainedParameterChildren.contains(RouteConstraint::String)) {
                childNode =
                    curr->constrainedParameterChildren.find(RouteConstraint::String)->second.get();
            } else {
                // not found for the given param types
                return RouteMatch(RoutingStatus::NotFound);
            }

            capturedValues.push_back(segmentKey);

            if (index == segmentsSize - 1) {
                if (childNode->endpointsByMethod.empty()) {
                    return RouteMatch(RoutingStatus::NotFound);
                }
                if (!childNode->endpointsByMethod.contains(method)) {
                    if (method == http::Method::Head &&
                        childNode->endpointsByMethod.contains(http::Method::Get)) {
                        auto endpoint =
                            childNode->endpointsByMethod.find(http::Method::Get)->second;
                        return RouteMatch(
                            endpoint,
                            std::move(
                                makeRouteParameters(endpoint->parameterNames, capturedValues)));
                    }

                    for (const auto& entry : childNode->endpointsByMethod) {
                        allowedMethods.insert(entry.first);
                    }

                    return RouteMatch(allowedMethods);
                }

                auto endpoint = childNode->endpointsByMethod.find(method)->second;

                return RouteMatch(
                    endpoint,
                    std::move(makeRouteParameters(endpoint->parameterNames, capturedValues)));
            }

            return matchRoute(
				method,
				segments,
				index + 1,
				capturedValues,
				allowedMethods,
				childNode // continue down the parameterized route
			);
        }

        return RouteMatch(RoutingStatus::NotFound);
    }

    void RouteTrie::debugDump() const {
        std::function<void(
            const RouteNode&,
            const std::string&,
            bool,
            bool,
            const std::optional<RouteConstraint>&)>
            print = [&](const RouteNode& node,
                        const std::string& prefix,
                        bool isLast,
                        bool isParam,
                        const std::optional<routing::RouteConstraint>& constraint) {
                const std::string connector = isLast ? "\\-- " : "|-- ";
                // Build label
                std::string label =
                    node.segmentKey.empty()
                        ? "[root]"
                        : (isParam
                               ? "{" + node.segmentKey +
                                     (constraint ? ":" + std::string(toString(*constraint)) : "") +
                                     "}"
                               : node.segmentKey);
                if (!node.endpointsByMethod.empty()) {
                    label += " [";
                    bool first = true;
                    for (const auto& [method, _] : node.endpointsByMethod) {
                        if (!first)
                            label += ", ";
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
                    print(
                        *node.childrenByStaticSegment.at(keys[i]),
                        childPrefix,
                        lastChild,
                        false,
                        std::nullopt);
                }
                // Collect and sort parameterized children by constraint name for stable output
                std::vector<std::optional<routing::RouteConstraint>> constraints;
                constraints.reserve(node.constrainedParameterChildren.size());
                for (const auto& [constraintKey, _] : node.constrainedParameterChildren)
                    constraints.push_back(constraintKey);

                std::sort(constraints.begin(), constraints.end(), [](const auto& a, const auto& b) {
                    const std::string_view sa = a ? toString(*a) : "";
                    const std::string_view sb = b ? toString(*b) : "";
                    return sa < sb;
                });

                for (size_t i = 0; i < constraints.size(); ++i) {
                    const bool lastChild = (i == constraints.size() - 1);
                    print(
                        *node.constrainedParameterChildren.at(constraints[i]),
                        childPrefix,
                        lastChild,
                        true,
                        constraints[i]);
                }
            };

        print(m_root, "", true, false, std::nullopt);
        std::cout << '\n';
    }
}
