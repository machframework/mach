#pragma once

#include <mach/detail/routing/Router.hpp>

namespace mach
{
    struct Context;
    class Next;
}

namespace mach::detail::routing
{
    class RoutingMiddleware {

    public:
        explicit RoutingMiddleware(Router& router) : m_router(router) {}

        void invoke(mach::Context& context, mach::Next& next);

    private:
        Router& m_router;
    };
}
