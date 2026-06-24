#pragma once

#include <concepts>
#include <type_traits>

#include <mach/Context.hpp>
#include <mach/middleware/Next.hpp>

namespace mach::detail::middleware
{
    template <typename T>
    concept MachMiddleware =
        std::is_class_v<T> &&
        !std::is_const_v<T> &&
        !std::is_reference_v<T> &&
        !std::is_pointer_v<T> &&
        requires(T middleware, mach::Context & ctx, mach::Next next) {
            { middleware.invoke(ctx, next) } -> std::same_as<void>;
    };
}
