#pragma once

#include <tuple>
#include <type_traits>

namespace mach::detail::traits
{
    template <typename T>
    concept HasSingleConcreteCallOperator = requires { &std::remove_cvref_t<T>::operator(); };

    template <typename T>
    struct FunctionTraits;

    // Functors and lambdas with a single, concrete operator().
    template <typename T>
        requires HasSingleConcreteCallOperator<T>
    struct FunctionTraits<T> : FunctionTraits<decltype(&std::remove_cvref_t<T>::operator())> {};

    // Free-function types.
    template <typename Return, typename... Args>
    struct FunctionTraits<Return(Args...)> {
        using ReturnType = Return;
        using ArgsTuple = std::tuple<Args...>;
    };

    // noexcept free-function types.
    template <typename Return, typename... Args>
    struct FunctionTraits<Return(Args...) noexcept> : FunctionTraits<Return(Args...)> {};

    // Function pointers.
    template <typename Return, typename... Args>
    struct FunctionTraits<Return (*)(Args...)> : FunctionTraits<Return(Args...)> {};

    // noexcept function pointers.
    template <typename Return, typename... Args>
    struct FunctionTraits<Return (*)(Args...) noexcept> : FunctionTraits<Return(Args...)> {};

    // Non-const call operators, including mutable lambdas.
    template <typename Class, typename Return, typename... Args>
    struct FunctionTraits<Return (Class::*)(Args...)> {
        using ClassType = Class;
        using ReturnType = Return;
        using ArgsTuple = std::tuple<Args...>;
    };

    // Const call operators, including ordinary lambdas.
    template <typename Class, typename Return, typename... Args>
    struct FunctionTraits<Return (Class::*)(Args...) const> {
        using ClassType = Class;
        using ReturnType = Return;
        using ArgsTuple = std::tuple<Args...>;
    };

    // noexcept non-const call operators.
    template <typename Class, typename Return, typename... Args>
    struct FunctionTraits<Return (Class::*)(Args...) noexcept>
        : FunctionTraits<Return (Class::*)(Args...)> {};

    // noexcept const call operators.
    template <typename Class, typename Return, typename... Args>
    struct FunctionTraits<Return (Class::*)(Args...) const noexcept>
        : FunctionTraits<Return (Class::*)(Args...) const> {};

    template <typename T>
    concept MinimalApiHandler = requires {
        typename FunctionTraits<std::remove_cvref_t<T>>::ReturnType;

        typename FunctionTraits<std::remove_cvref_t<T>>::ArgsTuple;
    };
}
