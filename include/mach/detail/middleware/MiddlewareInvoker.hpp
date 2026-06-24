#pragma once

#include <mach/detail/middleware/Next.hpp>

#include "IMiddlewareInvoker.hpp"

namespace mach::detail::middleware
{
	template <typename TMiddleware>
	class MiddlewareInvoker final : public IMiddlewareInvoker {

	public:
		void invoke(dispatching::RequestExecution& execution, middleware::Next next) override;
	};

	template <typename TMiddleware>
	void MiddlewareInvoker<TMiddleware>::invoke(dispatching::RequestExecution& execution, middleware::Next next) {
		auto middleware = execution.scope.resolve<TMiddleware>();
		
		middleware->invoke(
			execution.context,
			[&execution, next = std::move(next)] {
				next(execution);
			}
		);
	}
}
