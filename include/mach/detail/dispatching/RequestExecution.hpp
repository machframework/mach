#pragma once

#include <mach/detail/di/Scope.hpp>

namespace mach
{
    struct Context;
}

namespace mach::detail::dispatching
{
    struct RequestExecution {
        RequestExecution(mach::Context& context, di::Scope& scope)
            : context(context), scope(scope) {}

        mach::Context& context;
        di::Scope& scope;
    };
}
