#pragma once

namespace mach::detail::traits
{
    template <typename T>
    struct FunctionTraits : FunctionTraits<decltype(&T::operator())> {};

    template <typename Class, typename Return, typename... Args>
    struct FunctionTraits<Return(Class::*)(Args...)> {
        using ClassType = Class;
        using ReturnType = Return;
        using ArgsTuple = std::tuple<Args...>;
    };

    template <typename Class, typename Return, typename... Args>
    struct FunctionTraits<Return(Class::*)(Args...) const> {
        using ClassType = Class;
        using ReturnType = Return;
        using ArgsTuple = std::tuple<Args...>;
    };

    template <typename T>
    concept MinimalApiHandler =
        requires {
        typename FunctionTraits<std::remove_cvref_t<T>>::ReturnType;
        typename FunctionTraits<std::remove_cvref_t<T>>::ArgsTuple;
    };
}
