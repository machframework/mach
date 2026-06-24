#include <mach/detail/dispatching/Dispatcher.hpp>
#include <mach/detail/middleware/Next.hpp>
#include <mach/detail/routing/RouteEndpoint.hpp>

namespace mach::detail::dispatching
{
	Dispatcher::Dispatcher(di::Container& container, middleware::MiddlewarePipeline&& middlewarePipeline)
		: m_container(container),
		m_middlewarePipeline(std::move(middlewarePipeline))
	{ }

	void Dispatcher::execute(mach::Context& context, application::ExecutionPlan&& plan) {
		// construct execution chain

		// create scope
		auto scope = m_container.createScope();

		auto execution = RequestExecution(context, scope);

		auto terminal = [&plan](RequestExecution& execution) {
			plan.endpoint->invoker->invoke(execution);
		};

		m_middlewarePipeline.invoke(execution, terminal);
	}
}
