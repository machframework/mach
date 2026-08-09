#pragma once

#include <mach/detail/dispatching/RequestExecution.hpp>

namespace mach::detail::dispatching
{
    class IEndpointInvoker {

    public:
        virtual ~IEndpointInvoker() = default;
        virtual void invoke(RequestExecution& execution) = 0;
    };
}
