#include "Dispatcher.hpp"

namespace mach::detail::dispatching
{
	void Dispatcher::execute(mach::Context& context, application::ExecutionPlan&& plan) {
		// construct execution chain

		// execute handler
		plan.endpoint->handler(context);
	}
}
