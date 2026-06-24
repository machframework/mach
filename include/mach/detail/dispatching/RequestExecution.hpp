#pragma once

#include <mach/Context.hpp>
#include <mach/detail/di/Scope.hpp>

namespace mach::detail::dispatching
{
	struct RequestExecution {
		mach::Context& context;
		di::Scope& scope;

		RequestExecution(mach::Context& context, di::Scope& scope)
			: context(context),
			scope(scope)
		{ }
	};
}
