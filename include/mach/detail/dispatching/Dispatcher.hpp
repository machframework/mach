#pragma once

#include <mach/Context.hpp>

#include <mach/detail/application/ExecutionPlan.hpp>
#include <mach/detail/di/Container.hpp>

namespace mach::detail::dispatching
{
	class Dispatcher {
		
	public:
		explicit Dispatcher(di::Container& container);

		void execute(mach::Context& context, application::ExecutionPlan&& plan);

	private:
		di::Container& m_container;

	};
}
