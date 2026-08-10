#pragma once

#include <functional>
#include <tuple>
#include <utility>

#include <nlohmann/json.hpp>

#include <mach/exceptions/BadRequestException.hpp>
#include <mach/Response.hpp>
#include <mach/ValidationBuilder.hpp>

#include <mach/detail/binding/BodyBinder.hpp>
#include <mach/detail/binding/JsonConcepts.hpp>
#include <mach/detail/controllers/ControllerTraits.hpp>
#include <mach/detail/dispatching/IEndpointInvoker.hpp>
#include <mach/detail/http/ContentType.hpp>
#include <mach/detail/results/ResultTraits.hpp>
#include <mach/detail/serailization/Serializer.hpp>
#include <mach/detail/validation/ValidationUtils.hpp>

namespace mach::detail::dispatching
{
    template <typename TController, typename TResult, typename... TArgs>
    class ControllerActionInvoker final : public IEndpointInvoker {
        static_assert(
            mach::detail::controllers::MachController<TController>,
            "Mach error: ControllerActionInvoker requires a valid controller type.");

        static_assert(
            mach::detail::results::traits::ReplyResult<TResult>,
            "Mach error: ControllerActionInvoker requires a Reply result type.");

    public:
        using Action = TResult (TController::*)(TArgs...);
        using ArgsTuple = std::tuple<TArgs...>;

        explicit ControllerActionInvoker(Action action) : m_action(action) {}

        void invoke(RequestExecution& execution) override;

    private:
        Action m_action;
    };

    template <typename TController, typename TResult, typename... TArgs>
    void ControllerActionInvoker<TController, TResult, TArgs...>::invoke(
        RequestExecution& execution) {
        auto& context = execution.context;

        auto& controller = execution.scope.resolve<TController>();
        controller.setContext(context);

        constexpr std::size_t parameterCount = sizeof...(TArgs);

        static_assert(
            parameterCount <= 1,
            "Mach error: controller actions may accept at most one parameter, which is bound from "
            "the request body.");

        const auto& stringBody = context.request.body();

        if (const auto contentType = context.request.header("content-type"); !stringBody.empty() &&
            (!contentType ||
             !http::matchesMediaType(*contentType, "application/json") ||
             http::hasUnsupportedCharset(*contentType))) {
            context.response = mach::Response{context.request.version()};
            context.response.status(mach::http::StatusCode::UnsupportedMediaType);
            return;
        }

        if constexpr (parameterCount <= 1) {
            TResult res = [&]() -> TResult {
                if constexpr (parameterCount == 0) {
                    return std::invoke(m_action, controller);
                } else {
                    // TEMPORARY: body param is always first
                    using DeclaredBodyType = std::tuple_element_t<0, std::tuple<TArgs...>>;

                    using BodyType = std::remove_cvref_t<DeclaredBodyType>;

                    constexpr bool passedByValue = std::same_as<DeclaredBodyType, BodyType>;

                    constexpr bool jsonDeserializable = binding::JsonDeserializable<BodyType>;

                    static_assert(
                        passedByValue,
                        "Mach error: controller action body parameter must be passed by value.");

                    static_assert(
                        jsonDeserializable,
                        "Mach error: controller action parameter must be deserializable from "
                        "JSON.");

                    if constexpr (passedByValue && jsonDeserializable) {
                        auto& binder = execution.scope.resolve<binding::BodyBinder>();
                        BodyType body = binder.bind<BodyType>(stringBody);

                        if constexpr (requires(mach::ValidationBuilder<BodyType>& builder) {
                                          { body.validate(builder) } -> std::same_as<void>;
                                      }) {
                            mach::ValidationBuilder<BodyType> validationBuilder;
                            validation::ValidationResult validationResult;

                            body.validate(validationBuilder);
                            validationBuilder.validate(body, validationResult);

                            if (validationResult.hasErrors()) {
                                throw BadRequestException(makeValidationError(validationResult));
                            }
                        }

                        return std::invoke(m_action, controller, std::move(body));
                    }
                }
            }();

            context.response.status(res.statusCode());

            using ValueType = TResult::ValueType;

            if constexpr (!std::same_as<ValueType, void>) {
                if (res.hasValue()) {
                    context.response.body(serialization::Serializer::serialize(res.value()));
                }
            }
        }
    }

    template <typename TController, typename TResult, typename TArgsTuple>
    struct ControllerActionInvokerFromTuple;

    template <typename TController, typename TResult, typename... TArgs>
    struct ControllerActionInvokerFromTuple<TController, TResult, std::tuple<TArgs...>> {
        using Type = ControllerActionInvoker<TController, TResult, TArgs...>;
    };
}
