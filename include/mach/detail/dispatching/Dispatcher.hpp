#pragma once

#include <mach/detail/middleware/MiddlewarePipeline.hpp>

namespace mach
{
    struct Context;
}

namespace mach::detail::dispatching
{
    class Dispatcher {

    public:
        Dispatcher(di::Container& container, middleware::MiddlewarePipeline&& middlewarePipeline);

        void execute(mach::Context& context) const;

    private:
        di::Container& m_container;
        middleware::MiddlewarePipeline m_middlewarePipeline;
    };
}
