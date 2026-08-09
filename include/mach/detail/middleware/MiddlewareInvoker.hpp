#pragma once

#include <mach/middleware/Next.hpp>
#include <mach/detail/middleware/InternalNext.hpp>

#include "IMiddlewareInvoker.hpp"

namespace mach::detail::middleware
{
    template <typename TMiddleware>
    class MiddlewareInvoker final : public IMiddlewareInvoker {

    public:
        void invoke(dispatching::RequestExecution& execution, const InternalNext& next)
            override;
    };

    template <typename TMiddleware>
    void MiddlewareInvoker<TMiddleware>::invoke(
        dispatching::RequestExecution& execution,
        const middleware::InternalNext& next) {
        auto& middleware = execution.scope.resolve<TMiddleware>();

        mach::Next publicNext([&execution, next = next]() {
            next(execution);
        });

        middleware.invoke(execution.context, publicNext);
    }
}
