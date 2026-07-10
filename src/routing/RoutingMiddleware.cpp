#include <mach/detail/routing/RoutingMiddleware.hpp>

namespace mach::detail::routing
{
	void RoutingMiddleware::invoke(mach::Context& context, mach::Next next) {
		auto plan = m_router.route(context.request);

		if (!plan.found()) {
			auto statusCode = routing::toStatusCode(plan.status);

			context.response.status(statusCode);
			context.response.body(std::string(mach::http::reasonPhrase(statusCode)));

			std::string allow = "";

			for (const mach::http::Method method : mach::http::allMethods) {
				if (!plan.allowedMethods.contains(method)) {
					continue;
				}

				if (!allow.empty()) {
					allow += ", ";
				}

				allow += toString(method);
			}

			if (!allow.empty()) {
				context.response.setHeader("allow", allow);
			}

			return;
		}

		context.request.setRouteParams(std::move(plan.params));
		context.executionPlan = std::move(plan);

		next();
	}
}
