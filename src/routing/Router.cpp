#include "Router.hpp"

namespace mach::detail::routing
{
	application::ExecutionPlan Router::route(const mach::Request& request) const {		
        application::ExecutionPlan plan{};
        
        auto match = matchRoute(request);

        plan.status = match.status;
        plan.endpoint = match.endpoint;

        return plan;
    }

	void Router::addRoute(Endpoint&& route) {
        auto segments = splitToSegments(route.pattern); 

        m_endpoints.push_back(std::move(route));
        Endpoint* stored = &m_endpoints.back();

        m_routes.addRoute(std::move(segments), stored);
    }

	routing::RouteMatch Router::matchRoute(const mach::Request& request) const {
        auto segments = splitToSegments(request.target());
        return m_routes.matchRoute(request.method(), std::move(segments));
	}

	std::vector<std::string_view> Router::splitToSegments(std::string_view pattern) {
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
}
