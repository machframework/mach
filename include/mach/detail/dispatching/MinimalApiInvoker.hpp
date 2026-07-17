#pragma once

#include <utility>
#include <tuple>
#include <type_traits>

#include <mach/detail/dispatching/IEndpointInvoker.hpp>
#include <mach/detail/results/ResultTraits.hpp>
#include <mach/detail/core/TypeTraits.hpp>
#include <mach/detail/serailization/Serializer.hpp>

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
        constexpr std::size_t parameterCount = sizeof...(TArgs);

        static_assert(
            parameterCount <= 2,
            "Mach error: minimal API handlers currently support at most two parameters."
            );
        
        auto handlerCallback = [&]() -> TResult {
            if constexpr (sizeof...(TArgs) == 0) {
                return std::invoke(m_handler);
            }
            else if constexpr (sizeof...(TArgs) == 1) {
                using FirstType = std::tuple_element_t<0, std::tuple<TArgs...>>;
                using ValueType = std::remove_cvref_t<FirstType>;

                constexpr bool isValidContext =
                    std::same_as<FirstType, mach::Context&> ||
                    std::same_as<FirstType, const mach::Context&>;

                constexpr bool isContext =
                    std::same_as<ValueType, mach::Context>;

                if constexpr (isValidContext) {
                    return std::invoke(m_handler, execution.context);
                }
                else {
                    static_assert(
                        !isContext,
                        "Mach error: Context parameters must be passed as mach::Context& or const mach::Context&."
                        );

                    if constexpr (!isContext) {

                        static_assert(
                            binding::JsonDeserializable<ValueType>,
                            "Mach error: minimal API body parameter must be deserializable from JSON."
                            );

                        if constexpr (binding::JsonDeserializable<ValueType>) {
                            auto& binder = execution.scope.resolve<binding::BodyBinder>();
                            ValueType body = binder.bind<ValueType>(execution.context.request.body());

                            return std::invoke(m_handler, std::move(body));
                        }
                    }
                }
            }
            else if constexpr (sizeof...(TArgs) == 2) {
                using Arguments = std::tuple<TArgs...>;
                using FirstType = std::tuple_element_t<0, Arguments>;
                using SecondType = std::tuple_element_t<1, Arguments>;

                constexpr bool firstIsValidContext =
                    std::same_as<FirstType, mach::Context&> ||
                    std::same_as<FirstType, const mach::Context&>;

                constexpr bool secondIsValidContext =
                    std::same_as<SecondType, mach::Context&> ||
                    std::same_as<SecondType, const mach::Context&>;

                constexpr bool firstIsContext =
                    std::same_as<
                    std::remove_cvref_t<FirstType>,
                    mach::Context
                    >;

                constexpr bool secondIsContext =
                    std::same_as<
                    std::remove_cvref_t<SecondType>,
                    mach::Context
                    >;

                static_assert(
                    !firstIsContext || firstIsValidContext,
                    "Mach error: the first parameter must be passed as mach::Context& or const mach::Context&."
                    );

                static_assert(
                    !secondIsContext || secondIsValidContext,
                    "Mach error: the second parameter must be passed as mach::Context& or const mach::Context&."
                    );

                static_assert(
                    firstIsValidContext != secondIsValidContext,
                    "Mach error: minimal API handlers with two parameters must accept exactly one mach::Context& or const mach::Context&."
                    );

                if constexpr (firstIsValidContext != secondIsValidContext) {
                    auto& binder = execution.scope.resolve<binding::BodyBinder>();

                    if constexpr (firstIsValidContext) {
                        using BodyType = std::remove_cvref_t<SecondType>;

                        static_assert(
                            binding::JsonDeserializable<BodyType>,
                            "Mach error: the second parameter of a two-parameter minimal API handler must be deserializable from JSON."
                            );

                        if constexpr (binding::JsonDeserializable<BodyType>) {
                            BodyType body =
                                binder.bind<BodyType>(
                                    execution.context.request.body()
                                );

                            return std::invoke(
                                m_handler,
                                execution.context,
                                std::move(body)
                            );
                        }
                    }
                    else {
                        using BodyType = std::remove_cvref_t<FirstType>;

                        static_assert(
                            binding::JsonDeserializable<BodyType>,
                            "Mach error: the first parameter of a two-parameter minimal API handler must be deserializable from JSON."
                            );

                        if constexpr (binding::JsonDeserializable<BodyType>) {
                            BodyType body =
                                binder.bind<BodyType>(
                                    execution.context.request.body()
                                );

                            return std::invoke(
                                m_handler,
                                std::move(body),
                                execution.context
                            );
                        }
                    }
                }
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
