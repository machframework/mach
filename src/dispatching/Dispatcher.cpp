#include "Dispatcher.hpp"

#include <mach/detail/routing/RouteEndpoint.hpp>

namespace mach::detail::dispatching
{
	Dispatcher::Dispatcher(di::Container& container)
		: m_container(container)
	{ }

	void Dispatcher::execute(mach::Context& context, application::ExecutionPlan&& plan) {
		// construct execution chain

		// create scope
		auto scope = m_container.createScope();

		// execute handler based on type
		if (plan.endpoint->kind == routing::EndpointKind::MinimalApi) {
			plan.endpoint->handler(context);
		}
		else {
			plan.endpoint->controllerAction->invoke(context, scope);
		}
	}
}
