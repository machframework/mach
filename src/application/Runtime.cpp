#include "Runtime.hpp"

#include <mach/http/StatusCode.hpp>

#include "routing/RoutingStatus.hpp"
#include "ExecutionPlan.hpp"

namespace mach::detail::application {
	void Runtime::handle(mach::Context& context) {
		auto plan = m_router.route(context.request);
		if (!plan.found()) {
			auto statusCode = routing::toStatusCode(plan.status);

			context.response.status(statusCode);
			context.response.body(mach::http::reasonPhrase(statusCode));

			// for now, short-circuit request. Later will go into dispatcher and global middleware.
			return; 
		}
		
		// dispatcher
		m_dispatcher.execute(context, std::move(plan));

		// echo request body
		context.response.body(context.request.body());
	}

	void Runtime::addRoute(routing::Endpoint&& route) {
		m_router.addRoute(std::move(route));
	}
}
