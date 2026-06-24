#include "Runtime.hpp"

#include <exception>
#include <iostream>

#include <mach/http/StatusCode.hpp>

#include <mach/detail/application/ExecutionPlan.hpp>
#include <mach/detail/routing/RoutingStatus.hpp>

namespace mach::detail::application 
{
	Runtime::Runtime(
		routing::Router router,
		di::Container container,
		middleware::MiddlewarePipeline middlewarePipeline
	) 
		: m_router(std::move(router)),
		m_container(std::move(container)),
		m_dispatcher(m_container, std::move(middlewarePipeline))
	{ }

	void Runtime::handle(mach::Context& context) {
		auto plan = m_router.route(context.request);
		if (!plan.found()) {
			auto statusCode = routing::toStatusCode(plan.status);

			context.response.status(statusCode);
			context.response.body(mach::http::reasonPhrase(statusCode));

			// for now, short-circuit request. Later will go into dispatcher and global middleware.
			return; 
		}
		
		// add params to request object
		context.request.setRouteParams(std::move(plan.params));

		try {
			// dispatcher
			m_dispatcher.execute(context, std::move(plan));
		}
		catch (const std::exception& ex) {
			std::cout << "Error: " << ex.what() << std::endl;

			// error
			context.response.status(mach::http::StatusCode::InternalServerError);
			context.response.body(std::move(ex.what()));
		}
	}

	void Runtime::addRoute(routing::RouteEndpoint&& endpoint) {
		m_router.addRoute(std::move(endpoint));
	}
}
