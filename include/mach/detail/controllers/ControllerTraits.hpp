#pragma once

#include <concepts>
#include <string>
#include <type_traits>

#include <mach/controllers/ControllerBase.hpp>

namespace mach::detail::controllers
{
    template <typename T>
    concept HasPublicStaticRouteField =
        requires {
            { T::route } -> std::same_as<std::string&>;
    };

    template <typename T>
    concept ControllerType =
        std::derived_from<T, mach::ControllerBase>;

    template <typename T>
    concept ValidController =
        ControllerType<T> && HasPublicStaticRouteField<T>;
}
