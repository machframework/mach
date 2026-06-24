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

		auto execution = RequestExecution(context, scope);

		plan.endpoint->invoker->invoke(execution);
	}
}
