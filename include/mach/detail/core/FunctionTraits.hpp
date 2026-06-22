#pragma once

namespace mach::detail 
{

    template <typename T>
    struct FunctionTraits;

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

}
