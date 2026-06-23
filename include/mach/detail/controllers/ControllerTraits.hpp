#pragma once

#include <concepts>
#include <string>
#include <type_traits>

#include <mach/controllers/ControllerBase.hpp>

namespace mach
{
    template <typename TController>
    class ControllerBuilder;

    namespace detail::controllers
    {
        template <typename T>
        concept HasPublicStaticRouteField =
            requires {
                { T::route } -> std::convertible_to<std::string_view>;
        };

        template <typename T>
        concept ControllerType =
            std::is_class_v<T> &&
            std::derived_from<T, mach::ControllerBase>;

        template <typename T>
        concept MachController =
            ControllerType<T> &&
            HasPublicStaticRouteField<T> &&
            requires(mach::ControllerBuilder<T>&builder) {
                { T::configure(builder) } -> std::same_as<void>;
        };

        template <typename T>
        concept ValidController = MachController<T>;
    }
}
