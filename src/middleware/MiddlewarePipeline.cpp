#include <mach/detail/middleware/MiddlewarePipeline.hpp>

namespace mach::detail::middleware
{
	void MiddlewarePipeline::invoke(dispatching::RequestExecution& execution, middleware::Next terminal) const {
		auto current = std::move(terminal);

		for (auto it = m_middlewares.rbegin(); it != m_middlewares.rend(); ++it) {
			auto* middleware = it->get();
			auto next = std::move(current);

			current = [middleware, next = std::move(next)](
				dispatching::RequestExecution& execution
				) mutable {
					middleware->invoke(execution, next);
				};
		}

		current(execution);
	}
}
