#pragma once

#include <concepts>
#include <type_traits>

#include <mach/Context.hpp>
#include <mach/middleware/Next.hpp>

namespace mach::detail::traits::middleware
{
    template <typename T>
    concept ValidMiddlewareType = std::is_class_v<T> && std::same_as<T, std::remove_cvref_t<T>>;

    template <typename T>
    concept HasValidMiddlewareInvoke =
        requires { static_cast<void (T::*)(mach::Context&, mach::Next&)>(&T::invoke); };
}
