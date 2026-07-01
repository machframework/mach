#pragma once

#include <utility>
#include <tuple>
#include <type_traits>

#include <mach/detail/dispatching/IEndpointInvoker.hpp>
#include <mach/detail/results/ResultTraits.hpp>
#include <mach/detail/core/TypeTraits.hpp>

namespace mach::detail::dispatching
{
    template <
        typename THandler,
        typename TResult,
        typename... TArgs
    >
    class MinimalApiInvoker final : public IEndpointInvoker {

    public:
        using ArgsTuple = std::tuple<TArgs...>;

        explicit MinimalApiInvoker(THandler handler)
            : m_handler(std::move(handler))
        { }

        void invoke(RequestExecution& execution) const override;

    private:
        THandler m_handler;
    };

    template <
        typename THandler,
        typename TResult,
        typename... TArgs
    >
    void MinimalApiInvoker<THandler, TResult, TArgs...>::invoke(RequestExecution& execution) const {
        auto handlerCallback = [&]() -> TResult {
            if constexpr (sizeof...(TArgs) == 0) {
                return std::invoke(m_handler);
            }
            else if constexpr (sizeof...(TArgs) == 1) {
                using FirstType = std::tuple_element_t<0, std::tuple<TArgs...>>;

                if constexpr (std::same_as<std::remove_cvref_t<FirstType>, mach::Context>) {
                    return std::invoke(m_handler, execution.context);
                }
                else {
                    auto& binder = execution.scope.resolve<binding::BodyBinder>();
                    FirstType body = binder.bind<FirstType>(execution.context.request.body());

                    return std::invoke(m_handler, std::move(body));
                }
            }
            else if constexpr (sizeof...(TArgs) == 2) {
                using Arg0 = std::tuple_element_t<0, std::tuple<TArgs...>>;
                using Arg1 = std::tuple_element_t<1, std::tuple<TArgs...>>;

                // first is context, second is body
                if constexpr (
                    std::same_as<std::remove_cvref_t<Arg0>, mach::Context>
                    && !std::same_as<std::remove_cvref_t<Arg1>, mach::Context>
                    ) {
                    auto& binder = execution.scope.resolve<binding::BodyBinder>();
                    Arg1 body = binder.bind<Arg1>(execution.context.request.body());
                    return std::invoke(m_handler, execution.context, std::move(body));
                }
                else if constexpr (
                    std::same_as<std::remove_cvref_t<Arg1>, mach::Context>
                    && !std::same_as<std::remove_cvref_t<Arg0>, mach::Context>
                    ) {
                    auto& binder = execution.scope.resolve<binding::BodyBinder>();
                    Arg0 body = binder.bind<Arg0>(execution.context.request.body());
                    return std::invoke(m_handler, std::move(body), execution.context);
                }
            }
            else {
                static_assert(sizeof...(TArgs) <= 2, "Mach error: minimal APIs currently support at most one parameter.");
            }
            };

        if constexpr (std::same_as<TResult, void>) {
            handlerCallback();
        }
        else if constexpr (results::ReplyResult<TResult>) {
            TResult res = handlerCallback();

            execution.context.response.status(res.statusCode());
            if (res.hasValue()) {
                execution.context.response.body(std::move(serialization::Serializer::serialize(res.value())));
            }
        }
        else {
            static_assert(
                always_false_v<TResult>,
                "Mach error: minimal API handlers must return void or mach::Reply<T>."
            );
        }
    }

    template <typename THandler, typename TResult, typename TArgsTuple>
    struct MinimalApiInvokerFromTuple;

    template <typename THandler, typename TResult, typename... TArgs>
    struct MinimalApiInvokerFromTuple<
        THandler,
        TResult,
        std::tuple<TArgs...>
    > {
        using Type = MinimalApiInvoker<THandler, TResult, TArgs...>;
    };

    template <typename THandler, typename TResult, typename TArgsTuple>
    using MinimalApiInvokerFromTupleT =
        typename MinimalApiInvokerFromTuple<THandler, TResult, TArgsTuple>::Type;
}
