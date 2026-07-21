#pragma once

#include <tuple>
#include <type_traits>

namespace mach::detail::traits
{
    template <typename T>
    struct FunctionTraits;

    template <typename Class, typename Return, typename... Args>
    struct FunctionTraits<Return(Class::*)(Args...)>
    {
        using ClassType = Class;
        using ReturnType = Return;
        using ArgsTuple = std::tuple<Args...>;
    };

    template <typename Class, typename Return, typename... Args>
    struct FunctionTraits<Return(Class::*)(Args...) const>
    {
        using ClassType = Class;
        using ReturnType = Return;
        using ArgsTuple = std::tuple<Args...>;
    };

    template <typename T>
        requires requires {
        &std::remove_cvref_t<T>::operator();
    }
    struct FunctionTraits<T>
        : FunctionTraits<decltype(&std::remove_cvref_t<T>::operator())>
    {};

    template <typename T>
    concept MinimalApiHandler =
        requires {
        typename FunctionTraits<std::remove_cvref_t<T>>::ReturnType;
        typename FunctionTraits<std::remove_cvref_t<T>>::ArgsTuple;
    };
}