#pragma once

#include <mach/middleware/Next.hpp>

#include <mach/detail/dispatching/RequestExecution.hpp>

namespace mach::detail::middleware
{
	class IMiddlewareInvoker {
	
	public:
		virtual ~IMiddlewareInvoker() = default;
		virtual void invoke(dispatching::RequestExecution& execution, middleware::Next next) = 0;
	};
}
