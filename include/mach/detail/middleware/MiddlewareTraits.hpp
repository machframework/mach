#pragma once

#include <mach/Context.hpp>
#include <mach/middleware/Next.hpp>

namespace mach::detail::middleware
{
    template <typename T>
    concept MachMiddleware =
        requires(T middleware, mach::Context& ctx, mach::Next next) {
            { middleware.invoke(ctx, next) } -> std::same_as<void>;
    };
}
