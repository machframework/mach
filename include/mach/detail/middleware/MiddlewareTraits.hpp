#pragma once

#include <concepts>
#include <type_traits>

#include <mach/Context.hpp>
#include <mach/middleware/Next.hpp>

namespace mach::detail::traits::middleware
{
    template <typename T>
    concept ValidMiddlewareType =
        std::is_class_v<T> &&
        std::same_as<T, std::remove_cvref_t<T>>;

    template <typename T>
    concept HasValidMiddlewareInvoke =
        requires {
        static_cast<void (T::*)(mach::Context&, const mach::Next&)>(
            &T::invoke
            );
    };

    template <typename T>
    concept MachMiddleware =
        std::is_class_v<T> &&
        !std::is_const_v<T> &&
        !std::is_reference_v<T> &&
        !std::is_pointer_v<T> &&
        requires(T middleware, mach::Context & ctx, const mach::Next& next) {
            { middleware.invoke(ctx, next) } -> std::same_as<void>;
    };
}
