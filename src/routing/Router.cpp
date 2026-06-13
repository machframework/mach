#include "Router.hpp"

#include <format>
#include <iostream>
#include <stdexcept>
#include <unordered_set>

namespace
{
    std::vector<std::string_view> splitToSegments(std::string_view pattern) {
        std::vector<std::string_view> segments;

        const char delimiter = '/';
        std::size_t segmentStart = 0;

        for (std::size_t i = 0; i < pattern.size(); ++i) {
            if (pattern[i] == delimiter) {
                if (i > segmentStart) {
                    segments.emplace_back(
                        pattern.data() + segmentStart,
                        i - segmentStart
                    );
                }

                segmentStart = i + 1;
            }
        }

        if (segmentStart < pattern.size()) {
            segments.emplace_back(
                pattern.data() + segmentStart,
                pattern.size() - segmentStart
            );
        }

        return segments;
    }

    bool isParameter(std::string_view segment) {
        return segment.front() == '{'
            && segment.back() == '}';
    }

    std::vector<std::string_view> extractParameterSegments(const std::vector<std::string_view>& segments) {
        std::vector<std::string_view> parameters;

        for (const auto& segment : segments) {
            if (isParameter(segment)) {
                parameters.push_back(segment);
            }
        }

        return parameters;
    }

    bool containsDuplicateParameters(const std::vector<std::string_view>& parameters, std::string& duplicate) {
        std::unordered_set<std::string_view> seen;

        for (const auto& param : parameters) {
            if (!seen.insert(param).second) {
                duplicate = param;
                return true;
            }
        }

        return false;
    }

    std::string extractParameter(std::string_view segment) {
        segment.remove_prefix(1);
        segment.remove_suffix(1);

        return std::string(segment);
    }
}

namespace mach::detail::routing
{
	application::ExecutionPlan Router::route(const mach::Request& request) const {		
        application::ExecutionPlan plan{};
        
        auto match = matchRoute(request);

        plan.status = match.status;
        plan.endpoint = match.endpoint;
        plan.params = std::move(match.params);

        return plan;
    }

	void Router::addRoute(Endpoint&& endpoint) {
        auto segments = splitToSegments(endpoint.pattern); 

        std::string duplicate;
        if (containsDuplicateParameters(extractParameterSegments(segments), duplicate)) {
            throw std::invalid_argument(
                std::format(
                    "Duplicate route parameter '{}' in route '{}'",
                    extractParameter(duplicate),
                    endpoint.pattern
                )
            );
        }

        m_endpoints.push_back(std::move(endpoint));
        Endpoint* stored = &m_endpoints.back();

        m_routes.addRoute(std::move(segments), stored);
    }

	routing::RouteMatch Router::matchRoute(const mach::Request& request) const {
        auto segments = splitToSegments(request.target());
        return m_routes.matchRoute(request.method(), std::move(segments));
	}
}
