#pragma once

#include <mach/detail/dispatching/IEndpointInvoker.hpp>
#include <mach/detail/core/MinimalApiHandler.hpp>

namespace mach::detail::dispatching
{
    class MinimalApiInvoker final : public IEndpointInvoker {

    public:
        explicit MinimalApiInvoker(mach::detail::MinimalApiHandler handler)
            : m_handler(std::move(handler))
        {
        }

        void invoke(RequestExecution& execution) const override {
            m_handler(execution.context);
        }

    private:
        MinimalApiHandler m_handler;
    };
}
