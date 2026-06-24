#pragma once

#include <mach/Context.hpp>
#include <mach/detail/di/Scope.hpp>

namespace mach::detail::dispatching
{
    class IControllerActionInvoker {
    public:
        virtual ~IControllerActionInvoker() = default;

        virtual void invoke(Context& ctx, di::Scope& scope) const = 0;
    };
}