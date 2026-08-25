#include <mach/detail/routing/Router.hpp>

#include <algorithm>
#include <format>
#include <iostream>
#include <stdexcept>
#include <unordered_set>

#include <mach/Request.hpp>
#include <mach/exceptions/BadRequestException.hpp>

#include "utility/StringUtils.hpp"

namespace
{
    std::vector<std::string_view> splitToSegments(std::string_view pattern) {
        std::vector<std::string_view> segments;

        constexpr char delimiter = '/';
        std::size_t segmentStart = 0;

        for (std::size_t i = 0; i < pattern.size(); ++i) {
            if (pattern[i] == delimiter) {
                if (i > segmentStart) {
                    segments.emplace_back(pattern.data() + segmentStart, i - segmentStart);
                }

                segmentStart = i + 1;
            }
        }

        if (segmentStart < pattern.size()) {
            segments.emplace_back(pattern.data() + segmentStart, pattern.size() - segmentStart);
        }

        return segments;
    }

    bool isParameter(std::string_view segment) {
        return segment.find('{') != std::string_view::npos &&
               segment.find('}') != std::string_view::npos && segment.find('{') < segment.find('}');
    }

    std::vector<std::string_view> extractParameterSegments(
        const std::vector<std::string_view>& segments) {
        std::vector<std::string_view> parameters;

        for (const auto& segment : segments) {
            if (isParameter(segment)) {
                parameters.push_back(segment);
            }
        }

        return parameters;
    }

    std::string_view extractParameterName(std::string_view parameter) {
        const auto colon = parameter.find(':');

        return parameter.substr(
            1,
            (colon == std::string_view::npos ? parameter.size() - 1 : colon) - 1);
    }

    bool containsDuplicateParameters(
        const std::vector<std::string_view>& parameters,
        std::string_view& duplicate) {
        std::unordered_set<std::string_view> seen;

        for (const auto& param : parameters) {
            const auto name = extractParameterName(param);
            if (!seen.insert(name).second) {
                duplicate = param;
                return true;
            }
        }

        return false;
    }

    bool containsSpaces(const std::vector<std::string_view>& segments) {
        return std::ranges::any_of(segments, [](std::string_view segment) {
            return segment.find(' ') != std::string_view::npos;
        });
    }

    std::string extractParameter(std::string_view segment) {
        segment.remove_prefix(1);
        segment.remove_suffix(1);

        return std::string(segment);
    }

    bool validateQueryComponent(std::string_view component) {
        for (std::size_t i = 0; i < component.size(); ++i) {
            if (component[i] != '%') {
                continue;
            }

            if (i + 2 >= component.size()) {
                return false;
            }

            if (!std::isxdigit(static_cast<unsigned char>(component[i + 1])) ||
                !std::isxdigit(static_cast<unsigned char>(component[i + 2]))) {
                return false;
            }

            i += 2;
        }

        return true;
    }

    void decodeQueryComponent(std::string& value) {
        std::size_t write = 0;

        for (std::size_t read = 0; read < value.size(); ++read) {
            if (value[read] == '%') {
                const auto hi = value[read + 1];
                const auto lo = value[read + 2];

                auto hexToInt = [](char c) -> unsigned char {
                    if (c >= '0' && c <= '9')
                        return c - '0';
                    if (c >= 'A' && c <= 'F')
                        return c - 'A' + 10;
                    return c - 'a' + 10;
                };

                value[write++] = static_cast<char>((hexToInt(hi) << 4) | hexToInt(lo));

                read += 2;
            } else {
                value[write++] = value[read];
            }
        }

        value.resize(write);
    }

    bool hasBalancedBracesPerSegment(const std::vector<std::string_view>& segments) {
        for (const auto segment : segments) {
            int depth = 0;

            for (const char ch : segment) {
                if (ch == '{') {
                    ++depth;
                } else if (ch == '}') {
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

    bool hasNestedBraces(const std::vector<std::string_view>& segments) {
        for (const auto segment : segments) {
            int depth = 0;

            for (const char ch : segment) {
                if (ch == '{') {
                    ++depth;

                    if (depth > 1) {
                        return true;
                    }
                } else if (ch == '}') {
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

    bool parameterOccupiesEntireSegment(std::string_view segment) {
        return segment.size() >= 2 && segment.front() == '{' &&
               segment.find('}') == segment.length() - 1;
    }

    bool parametersOccupyEntireSegments(
        const std::vector<std::string_view>& segments,
        std::string_view& invalid) {
        for (const auto& segment : segments) {
            if (isParameter(segment) && !parameterOccupiesEntireSegment(segment)) {
                invalid = segment;
                return false;
            }
        }
        return true;
    }

    std::unordered_map<std::string, std::string> parseQuery(std::string_view query) {
        std::unordered_map<std::string, std::string> queryParams;

        for (const auto queries = mach::detail::split(query, '&'); auto param : queries) {
            if (param.empty()) {
                continue;
            }

            std::string name;
            std::string value;

            if (const auto equalsPos = param.find('='); equalsPos != std::string_view::npos) {
                name = param.substr(0, equalsPos);
                value = param.substr(equalsPos + 1);
            } else {
                name = param;
                value = {};
            }

            if (!validateQueryComponent(name) || !validateQueryComponent(value)) {
                throw mach::BadRequestException("Malformed query parameter.");
            }

            decodeQueryComponent(name);
            decodeQueryComponent(value);

            queryParams.emplace(name, value);
        }

        return queryParams;
    }
}

namespace mach::detail::routing
{
    application::ExecutionPlan Router::route(mach::Request& request) const {
        application::ExecutionPlan plan{};

        auto match = matchRoute(request);

        plan.status = match.status;
        plan.endpoint = match.endpoint;
        plan.params = std::move(match.params);
        plan.allowedMethods = std::move(match.allowedMethods);

        return plan;
    }

    void Router::mapRoute(RouteEndpoint&& endpoint) {
        // enforce syntax
        auto& pattern = endpoint.pattern;
        std::string_view invalid;

        if (pattern.empty()) {
            throw std::invalid_argument(
                std::format("Invalid route definition: Route cannot be empty"));
        }
        if (pattern.front() != '/') {
            throw std::invalid_argument(
                std::format("Invalid route definition '{}': Route must begin with '/'", pattern));
        }
        if (containsRepeatedSlash(pattern)) {
            throw std::invalid_argument(
                std::format(
                    "Invalid route definition '{}': Route must not contain consecutive  '/' "
                    "characters",
                    pattern));
        }
        if (pattern.length() != 1 && pattern.back() == '/') {
            // remove trailing slash
            pattern.pop_back();
        }
        if (pattern.find('#') != std::string::npos || pattern.find('?') != std::string::npos) {
            throw std::invalid_argument(
                std::format(
                    "Invalid route definition '{}': Route must not contain '?' or '#'",
                    pattern));
        }

        auto segments = splitToSegments(pattern);

        if (containsSpaces(segments)) {
            throw std::invalid_argument(
                std::format(
                    "Invalid route definition '{}': Route must not contain whitespace",
                    pattern));
        }
        if (!hasBalancedBracesPerSegment(segments)) {
            throw std::invalid_argument(
                std::format(
                    "Invalid route definition '{}': Route must contain balanced braces",
                    pattern));
        }
        if (hasNestedBraces(segments)) {
            throw std::invalid_argument(
                std::format(
                    "Invalid route definition '{}': Route must not contain nested braces",
                    pattern));
        }
        if (!parametersOccupyEntireSegments(segments, invalid)) {
            throw std::invalid_argument(
                std::format(
                    "Invalid route definition '{}': Route parameters must occupy an entire path "
                    "segment",
                    pattern));
        }

        const auto parameters = extractParameterSegments(segments);

        if (emptyParameter(parameters, invalid)) {
            throw std::invalid_argument(
                std::format(
                    "Invalid route definition '{}': Route parameter name cannot be empty",
                    endpoint.pattern));
        }

        if (containsDuplicateParameters(parameters, invalid)) {
            throw std::invalid_argument(
                std::format(
                    "Invalid route definition '{}': Duplicate route parameter '{}'",
                    endpoint.pattern,
                    extractParameter(invalid)));
        }

        m_endpoints.push_back(std::move(endpoint));
        RouteEndpoint* stored = &m_endpoints.back();

        m_routes.mapRoute(std::move(segments), stored);
    }

    RouteMatch Router::matchRoute(mach::Request& request) const {
        request.setRouteQuery(extractQuery(request.m_target));
        auto segments = splitToSegments(request.target());

        return m_routes.matchRoute(request.method(), std::move(segments));
    }

    std::unordered_map<std::string, std::string> Router::extractQuery(std::string& target) {
        const auto queryPos = target.find('?');
        if (queryPos == std::string_view::npos) {
            return {};
        }

        const auto query = target.substr(queryPos + 1);
        target = target.substr(0, queryPos);

        return parseQuery(query);
    }
}
