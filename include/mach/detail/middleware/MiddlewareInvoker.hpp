#pragma once

#include "IMiddlewareInvoker.hpp"

namespace mach::detail::middleware
{
	template <typename TMiddleware>
	class MiddlewareInvoker final : public IMiddlewareInvoker {

	public:
		void invoke(dispatching::RequestExecution& execution, mach::Next next) override;
	};

	template <typename TMiddleware>
	void MiddlewareInvoker<TMiddleware>::invoke(dispatching::RequestExecution& execution, mach::Next next) {
		auto middleware = execution.scope.resolve<TMiddleware>();
		middleware->invoke(execution.context, std::move(next));
	}
}
