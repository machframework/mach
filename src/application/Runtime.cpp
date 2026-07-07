#include "Runtime.hpp"

#include <exception>
#include <iostream>

#include <mach/http/StatusCode.hpp>
#include <mach/logging/Logging.hpp>

#include <mach/detail/application/ExecutionPlan.hpp>
#include <mach/detail/routing/RoutingStatus.hpp>

namespace mach::detail::application 
{
	Runtime::Runtime(
		di::Container container,
		middleware::MiddlewarePipeline middlewarePipeline
	) 
		: m_container(std::move(container)),
		m_dispatcher(m_container, std::move(middlewarePipeline))
	{ }

	void Runtime::handle(mach::Context& context) {
		try {
			// dispatcher
			m_dispatcher.execute(context);
		}
		catch (const std::exception& ex) {
			logging::Logger::error(ex.what());

			// error
			context.response.status(mach::http::StatusCode::InternalServerError);
			context.response.body(
				std::string(mach::http::reasonPhrase(mach::http::StatusCode::InternalServerError))
			);
		}
	}
}
