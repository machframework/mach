#pragma once

#include <utility>

#include <mach/Context.hpp>
#include <mach/middleware/Next.hpp>

#include <mach/detail/routing/Router.hpp>

namespace mach::detail::routing
{
	class RoutingMiddleware {

	public:
		explicit RoutingMiddleware(Router& router)
			: m_router(router)
		{ }

		void invoke(mach::Context& context, mach::Next next) {
			auto plan = m_router.route(context.request);

			if (!plan.found()) {
				auto statusCode = routing::toStatusCode(plan.status);

				context.response.status(statusCode);
				context.response.body(mach::http::reasonPhrase(statusCode));

				return;
			}

			context.request.setRouteParams(std::move(plan.params));
			context.executionPlan = std::move(plan);

			next();
		}
	private:
		Router& m_router;
	};
}
