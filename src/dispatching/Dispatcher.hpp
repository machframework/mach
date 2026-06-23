#pragma once

#include <mach/Context.hpp>

#include <mach/detail/application/ExecutionPlan.hpp>

namespace mach::detail::dispatching
{
	class Dispatcher {
		
	public:
		void execute(mach::Context& context, application::ExecutionPlan&& plan);

	private:

	};
}
