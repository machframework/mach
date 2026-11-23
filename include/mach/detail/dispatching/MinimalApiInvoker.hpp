#pragma once

#include <tuple>
#include <type_traits>
#include <utility>

#include <mach/ValidationBuilder.hpp>
#include <mach/exceptions/BadRequestException.hpp>

#include <mach/detail/binding/BindingConcepts.hpp>
#include <mach/detail/binding/BindingTraits.hpp>

#include <mach/detail/core/TypeTraits.hpp>
#include <mach/detail/dispatching/IEndpointInvoker.hpp>
#include <mach/detail/http/ContentType.hpp>
#include <mach/detail/results/ResultTraits.hpp>
#include <mach/detail/serialization/Serializer.hpp>
#include <mach/detail/validation/ValidationResult.hpp>
#include <mach/detail/validation/ValidationUtils.hpp>

namespace mach::detail::dispatching
{
    template <typename Tuple>
    consteval bool expectsBody() {
        if constexpr (
            constexpr std::size_t parameterCount = std::tuple_size_v<Tuple>; parameterCount == 0
        ) {
            return false;
        } else if constexpr (parameterCount == 1) {
            using Arg = std::tuple_element_t<0, Tuple>;

            constexpr bool isValidContext =
                std::same_as<Arg, mach::Context&> || std::same_as<Arg, const mach::Context&>;

            return !isValidContext;
        } else {
            return true;
        }
    }

    template <typename THandler, typename TResult, typename... TArgs>
    class MinimalApiInvoker final : public IEndpointInvoker {

    public:
        using ArgsTuple = std::tuple<TArgs...>;

        explicit MinimalApiInvoker(THandler handler) : m_handler(std::move(handler)) {}

        void invoke(RequestExecution& execution) override;

    private:
        THandler m_handler;

        template <typename T>
        static constexpr bool isContext =
            std::same_as<T, mach::Context&> || std::same_as<T, const mach::Context&>;

        static constexpr bool usesExplicitBindings = (binding::BindingArgument<TArgs> || ...);

        template <typename T>
        static decltype(auto) resolveArgument(
            mach::Context& context,
            binding::RequestBinder& binder);
    };

    template <typename THandler, typename TResult, typename... TArgs>
    void MinimalApiInvoker<THandler, TResult, TArgs...>::invoke(RequestExecution& execution) {
        constexpr std::size_t parameterCount = sizeof...(TArgs);

        auto handlerCallback = [&]() -> TResult {
            if constexpr (usesExplicitBindings) {
                static_assert(
                    ((isContext<TArgs> || (mach::detail::binding::Binding<TArgs> &&
                                           std::same_as<TArgs, std::remove_cvref_t<TArgs>>)) &&
                     ...),
                    "Handler arguments must be Context&, const Context&, "
                    "or unqualified binding types passed by value.");

                static_assert(
                    (0 + ... + static_cast<std::size_t>(isContext<TArgs>)) <= 1,
                    "Handler must not accept more than one Context parameter.");

                if constexpr (parameterCount == 0) {
                    return std::invoke(m_handler);
                } else {
                    auto& binder = execution.scope.resolve<binding::RequestBinder>();
                    std::tuple<TArgs...> args{resolveArgument<TArgs>(execution.context, binder)...};

                    return std::apply(m_handler, std::move(args));
                }
            } else {
                if constexpr (parameterCount == 0) {
                    return std::invoke(m_handler);
                } else if constexpr (parameterCount == 1) {
                    using FirstType = std::tuple_element_t<0, std::tuple<TArgs...>>;
                    using ValueType = std::remove_cvref_t<FirstType>;

                    constexpr bool isValidContext = std::same_as<FirstType, mach::Context&> ||
                                                    std::same_as<FirstType, const mach::Context&>;

                    constexpr bool isContext = std::same_as<ValueType, mach::Context>;

                    if constexpr (isValidContext) {
                        return std::invoke(m_handler, execution.context);
                    } else {
                        static_assert(
                            !isContext,
                            "Mach error: Context parameters must be passed as mach::Context& or "
                            "const "
                            "mach::Context&.");

                        if constexpr (!isContext) {
                            static_assert(
                                binding::JsonDeserializable<ValueType>,
                                "Mach error: minimal API body parameter must be deserializable "
                                "from "
                                "JSON.");

                            if constexpr (binding::JsonDeserializable<ValueType>) {
                                auto& binder = execution.scope.resolve<binding::RequestBinder>();
                                ValueType body =
                                    binder.bind<ValueType>(execution.context.request);

                                if constexpr (
                                    requires(mach::ValidationBuilder<ValueType>& builder) {
                                        { body.validate(builder) } -> std::same_as<void>;
                                    }
                                ) {
                                    mach::ValidationBuilder<ValueType> validationBuilder;
                                    validation::ValidationResult validationResult;

                                    body.validate(validationBuilder);
                                    validationBuilder.validate(body, validationResult);

                                    if (validationResult.hasErrors()) {
                                        throw BadRequestException(
                                            makeValidationError(validationResult));
                                    }
                                }

                                return std::invoke(m_handler, std::move(body));
                            }
                        }
                    }
                } else if constexpr (sizeof...(TArgs) == 2) {
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
                        std::same_as<std::remove_cvref_t<FirstType>, mach::Context>;

                    constexpr bool secondIsContext =
                        std::same_as<std::remove_cvref_t<SecondType>, mach::Context>;

                    static_assert(
                        !firstIsContext || firstIsValidContext,
                        "Mach error: the first parameter must be passed as mach::Context& or const "
                        "mach::Context&.");

                    static_assert(
                        !secondIsContext || secondIsValidContext,
                        "Mach error: the second parameter must be passed as mach::Context& or "
                        "const "
                        "mach::Context&.");

                    static_assert(
                        firstIsValidContext != secondIsValidContext,
                        "Mach error: minimal API handlers with two parameters must accept exactly "
                        "one "
                        "mach::Context& or const mach::Context&.");

                    if constexpr (firstIsValidContext != secondIsValidContext) {
                        auto& binder = execution.scope.resolve<binding::RequestBinder>();

                        if constexpr (firstIsValidContext) {
                            using DeclaredBodyType = std::tuple_element_t<1, std::tuple<TArgs...>>;

                            using BodyType = std::remove_cvref_t<DeclaredBodyType>;

                            constexpr bool passedByValue = std::same_as<DeclaredBodyType, BodyType>;

                            static_assert(
                                passedByValue,
                                "Mach error: body parameter must be passed by value.");

                            static_assert(
                                binding::JsonDeserializable<BodyType>,
                                "Mach error: the second parameter of a two-parameter minimal API "
                                "handler must be deserializable from JSON.");

                            if constexpr (passedByValue && binding::JsonDeserializable<BodyType>) {
                                BodyType body =
                                    binder.bind<BodyType>(execution.context.request);
                                return std::invoke(m_handler, execution.context, std::move(body));
                            }
                        } else {
                            using DeclaredBodyType = std::tuple_element_t<0, std::tuple<TArgs...>>;

                            using BodyType = std::remove_cvref_t<DeclaredBodyType>;

                            constexpr bool passedByValue = std::same_as<DeclaredBodyType, BodyType>;

                            static_assert(
                                passedByValue,
                                "Mach error: body parameter must be passed by value.");

                            static_assert(
                                binding::JsonDeserializable<BodyType>,
                                "Mach error: the first parameter of a two-parameter minimal API "
                                "handler must be deserializable from JSON.");

                            if constexpr (passedByValue && binding::JsonDeserializable<BodyType>) {
                                BodyType body =
                                    binder.bind<BodyType>(execution.context.request.body());
                                return std::invoke(m_handler, std::move(body), execution.context);
                            }
                        }
                    }
                }
            }
        };

        if constexpr (std::same_as<TResult, void>) {
            handlerCallback();
        } else if constexpr (results::traits::ReplyResult<TResult>) {
            TResult res = handlerCallback();

            execution.context.response.status(res.statusCode());

            using ValueType = TResult::ValueType;

            if constexpr (!std::same_as<ValueType, void>) {
                if (res.hasValue()) {
                    execution.context.response.body(
                        serialization::Serializer::serialize(res.value()));
                }
            }
        } else {
            static_assert(
                detail::traits::always_false_v<TResult>,
                "Mach error: minimal API handlers must return void or mach::Reply<T>.");
        }
    }

    template <typename THandler, typename TResult, typename... TArgs>
    template <typename T>
    decltype(auto) MinimalApiInvoker<THandler, TResult, TArgs...>::resolveArgument(
        mach::Context& context,
        binding::RequestBinder& binder) {
        if constexpr (isContext<T>) {
            return static_cast<T>(context);
        } else {
            static_assert(
                binding::Binding<T>,
                "Handler argument must be Context& or const Context& or a binding type.");

            return binder.bind<T>(context.request);
        }
    }

    template <typename THandler, typename TResult, typename TArgsTuple>
    struct MinimalApiInvokerFromTuple;

    template <typename THandler, typename TResult, typename... TArgs>
    struct MinimalApiInvokerFromTuple<THandler, TResult, std::tuple<TArgs...>> {
        using Type = MinimalApiInvoker<THandler, TResult, TArgs...>;
    };

    template <typename THandler, typename TResult, typename TArgsTuple>
    using MinimalApiInvokerFromTupleT =
        MinimalApiInvokerFromTuple<THandler, TResult, TArgsTuple>::Type;
}
