#pragma once

#include <mach/Context.hpp>

#include <mach/detail/application/ExecutionPlan.hpp>
#include <mach/detail/di/Container.hpp>
#include <mach/detail/middleware/MiddlewarePipeline.hpp>

namespace mach::detail::dispatching
{
	class Dispatcher {
		
	public:
		Dispatcher(di::Container& container, middleware::MiddlewarePipeline&& middlewarePipeline);

		void execute(mach::Context& context, application::ExecutionPlan&& plan);

	private:
		di::Container& m_container;
		middleware::MiddlewarePipeline m_middlewarePipeline;
	};
}
