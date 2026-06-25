#pragma once

#include <tuple>
#include <utility>

#include <mach/detail/dispatching/IEndpointInvoker.hpp>
#include <mach/detail/results/ResultTraits.hpp>

namespace mach::detail::dispatching
{
    template <
        typename THandler,
        mach::detail::results::ReplyResult TResult,
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
        mach::detail::results::ReplyResult TResult,
        typename... TArgs
    >
    void MinimalApiInvoker<THandler, TResult, TArgs...>::invoke(RequestExecution& execution) const {
        TResult res = [&]() -> TResult {
            if constexpr (sizeof...(TArgs) == 0) {
                return std::invoke(m_handler);
            }
            else if constexpr (sizeof...(TArgs) == 1) {
                using BodyType = std::tuple_element_t<0, std::tuple<TArgs...>>;

                if constexpr (std::same_as<std::remove_cvref_t<BodyType>, mach::Context>) {
                    return std::invoke(m_handler, execution.context);
                }
                else {
                    auto& binder = execution.scope.resolve<binding::BodyBinder>();
                    BodyType body = binder.bind<BodyType>(execution.context.request.body());

                    return std::invoke(m_handler, body);
                }
            }
            else {
                static_assert(sizeof...(TArgs) <= 1, "Mach error: minimal APIs currently support at most one parameter.");
            }
            }();

        execution.context.response.status(res.statusCode());
        if (res.hasValue()) {
            execution.context.response.body(std::move(serialization::Serializer::serialize(res.value())));
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
