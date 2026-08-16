#include <mach/detail/middleware/MiddlewarePipeline.hpp>

namespace mach::detail::middleware
{
    void MiddlewarePipeline::invoke(
        dispatching::RequestExecution& execution,
        const InternalNext& terminal) const {
        auto current = terminal;

        for (auto it = m_middlewares.rbegin(); it != m_middlewares.rend(); ++it) {
            auto* middleware = it->get();
            auto next = std::move(current);

            current = [middleware, next = std::move(next)](
                          dispatching::RequestExecution& requestExecution) mutable {
                middleware->invoke(requestExecution, next);
            };
        }

        current(execution);
    }
}
