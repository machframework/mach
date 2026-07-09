#include <mach/detail/routing/Router.hpp>

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

    //bool isParameter(std::string_view segment) {
    //    return segment.front() == '{'
    //        && segment.back() == '}';
    //}

    bool isParameter(std::string_view segment) {
        return segment.find('{') != std::string_view::npos
            && segment.find('}') != std::string_view::npos
            && segment.find('{') < segment.find('}');
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

    bool containsDuplicateParameters(const std::vector<std::string_view>& parameters, std::string_view& duplicate) {
        std::unordered_set<std::string_view> seen;

        for (const auto& param : parameters) {
            if (!seen.insert(param).second) {
                duplicate = param;
                return true;
            }
        }

        return false;
    }

    bool containsSpaces(const std::vector<std::string_view>& segments) {
        for (const auto& segment : segments) {
            if (segment.find(' ') != std::string_view::npos) {
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

    bool hasBalancedBracesPerSegment(const std::vector<std::string_view>& segments)
    {
        for (const auto segment : segments) {
            int depth = 0;

            for (const char ch : segment) {
                if (ch == '{') {
                    ++depth;
                }
                else if (ch == '}') {
                    --depth;

                    if (depth < 0) {
                        return false;
                    }
                }
            }

            if (depth != 0) {
                return false;
            }
        }

        return true;
    }

    bool hasNestedBraces(const std::vector<std::string_view>& segments)
    {
        for (const auto segment : segments) {
            int depth = 0;

            for (const char ch : segment) {
                if (ch == '{') {
                    ++depth;

                    if (depth > 1) {
                        return true;
                    }
                }
                else if (ch == '}') {
                    --depth;
                }
            }
        }

        return false;
    }

    bool emptyParameter(const std::vector<std::string_view>& parameters, std::string_view& empty) {
        for (const auto& param : parameters) {
            if (param.length() == 2) {
                empty = param;
                return true;
            }
        }

        return false;
    }

    bool containsRepeatedSlash(std::string_view pattern) {
        char prev{};
        for (const auto ch : pattern) {
            if (prev == '/' && ch == '/') {
                return true;
            }

            prev = ch;
        }

        return false;
    }

    bool parameterOccupiesEntireSegment(std::string_view segment)
    {
        return segment.size() >= 2 &&
            segment.front() == '{' &&
            segment.find('}') == segment.length() - 1;
    }

	bool parametersOccupyEntireSegments(const std::vector<std::string_view>& segments, std::string_view& invalid)
	{
		for (const auto& segment : segments) {
			if (isParameter(segment) && !parameterOccupiesEntireSegment(segment)) {
				invalid = segment;
				return false;
			}
		}
		return true;
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

	void Router::addRoute(RouteEndpoint&& endpoint) {
        // enforce syntax
        const auto& pattern = endpoint.pattern;
        std::string_view invalid;

        if (pattern.empty()) {
            throw std::invalid_argument(
                std::format("Invalid route definition: Route cannot be empty")
            );
        }
        if (pattern.front() != '/') {
            throw std::invalid_argument(
                std::format("Invalid route definition '{}': Route must begin with '/'", pattern)
            );
        }
        if (pattern.length() != 1 && pattern.back() == '/') {
            throw std::invalid_argument(
                std::format("Invalid route definition '{}': Route must not end with '/'", pattern)
            );
        }
        if (containsRepeatedSlash(pattern)) {
            throw std::invalid_argument(
                std::format("Invalid route definition '{}': Route must not contain consecutive  '/' characters", pattern)
            );
        }
        if (pattern.find('#') != std::string::npos || pattern.find('?') != std::string::npos) {
            throw std::invalid_argument(
                std::format("Invalid route definition '{}': Route must not contain '?' or '#'", pattern)
            );
        }

        auto segments = splitToSegments(pattern); 

        if (containsSpaces(segments)) {
            throw std::invalid_argument(
                std::format("Invalid route definition '{}': Route must not contain whitespace", pattern)
            );
        }
        if (!hasBalancedBracesPerSegment(segments)) {
            throw std::invalid_argument(
                std::format("Invalid route definition '{}': Route must contain balanced braces", pattern)
            );
        }
        if (hasNestedBraces(segments)) {
			throw std::invalid_argument(
				std::format("Invalid route definition '{}': Route must not contain nested braces", pattern)
			);
        }
        if (!parametersOccupyEntireSegments(segments, invalid)) {
            throw std::invalid_argument(
                std::format("Invalid route definition '{}': Route parameters must occupy an entire path segment", pattern)
            );
        }

        const auto parameters = extractParameterSegments(segments);

        if (emptyParameter(parameters, invalid)) {
            throw std::invalid_argument(
                std::format(
                    "Invalid route definition '{}': Route parameter name cannot be empty",
                    endpoint.pattern
                )
            );
        }

        if (containsDuplicateParameters(parameters, invalid)) {
            throw std::invalid_argument(
                std::format(
                    "Invalid route definition '{}': Duplicate route parameter '{}'",
                    endpoint.pattern,
                    extractParameter(invalid)
                )
            );
        }

        m_endpoints.push_back(std::move(endpoint));
        RouteEndpoint* stored = &m_endpoints.back();

        m_routes.addRoute(std::move(segments), stored);
    }

	routing::RouteMatch Router::matchRoute(const mach::Request& request) const {
        auto segments = splitToSegments(request.target());
        return m_routes.matchRoute(request.method(), std::move(segments));
	}
}
