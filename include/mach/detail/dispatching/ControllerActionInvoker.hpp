#pragma once

#include <functional>
#include <memory>
#include <tuple>
#include <typeindex>
#include <utility>

#include <mach/detail/binding/BodyBinder.hpp>
#include <mach/detail/controllers/ControllerTraits.hpp>
#include <mach/detail/dispatching/IEndpointInvoker.hpp>
#include <mach/detail/results/ResultTraits.hpp>
#include <mach/detail/serailization/Serializer.hpp>

namespace mach::detail::dispatching 
{
    template <
        mach::detail::controllers::MachController TController,
        mach::detail::results::ReplyResult TResult,
        typename... TArgs
    >
    class ControllerActionInvoker final : public IEndpointInvoker {
    public:
        using Action = TResult (TController::*)(TArgs...);
        using ArgsTuple = std::tuple<TArgs...>;

        explicit ControllerActionInvoker(Action action)
            : m_action(action) {
        }

        void invoke(RequestExecution& execution) const override;

    private:
        Action m_action;
    };

    template <
        detail::controllers::MachController TController,
        detail::results::ReplyResult TResult,
        typename... TArgs
    >
    void ControllerActionInvoker<TController, TResult, TArgs...>::invoke(RequestExecution& execution) const {
        auto& controller = execution.scope.resolve<TController>();
        controller.context = &execution.context;

        constexpr std::size_t parameterCount = sizeof...(TArgs);

        static_assert(
            parameterCount <= 1,
            "Mach error: controller actions may accept at most one parameter, which is bound from the request body."
            );

        if constexpr (parameterCount <= 1) {
            TResult res = [&]() -> TResult {
                if constexpr (parameterCount == 0) {
                    return std::invoke(m_action, controller);
                }
                else {
                    // TEMPORARY: body param is always first
                    using BodyType = std::tuple_element_t<0, std::tuple<TArgs...>>;

                    auto& binder = execution.scope.resolve<binding::BodyBinder>();
                    BodyType body = binder.bind<BodyType>(execution.context.request.body());

                    return std::invoke(m_action, controller, std::move(body));
                }
                }();

            execution.context.response.status(res.statusCode());

            using ValueType = typename TResult::ValueType;

            if constexpr (!std::same_as<ValueType, void>) {
                if (res.hasValue()) {
                    execution.context.response.body(
                        serialization::Serializer::serialize(res.value())
                    );
                }
            }
        }
    }

    template <typename TController, typename TResult, typename TArgsTuple>
    struct ControllerActionInvokerFromTuple;

    template <typename TController, typename TResult, typename... TArgs>
    struct ControllerActionInvokerFromTuple<
        TController,
        TResult,
        std::tuple<TArgs...>
    > {
        using Type = ControllerActionInvoker<TController, TResult, TArgs...>;
    };
}
