#pragma once

#include <functional>
#include <memory>
#include <tuple>
#include <typeindex>

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
        mach::detail::controllers::MachController TController,
        mach::detail::results::ReplyResult TResult,
        typename... TArgs
    >
    void ControllerActionInvoker<TController, TResult, TArgs...>::invoke(RequestExecution& execution) const {
        auto& controller = execution.scope.resolve<TController>();
        controller.context = &execution.context;

        TResult res = [&]() -> TResult {
            if constexpr (sizeof...(TArgs) == 0) {
                return std::invoke(m_action, controller);
            }
            else {
                // TEMPORARY: body param is always first
                using BodyType = std::tuple_element_t<0, std::tuple<TArgs...>>;

                auto& binder = execution.scope.resolve<binding::BodyBinder>();
                BodyType body = binder.bind<BodyType>(execution.context.request.body());

                return std::invoke(m_action, controller, body);
            }
            }();

        /*TResult res;

        if constexpr (sizeof...(TArgs) == 0) {
            res = (controller.*m_action)();
        }
        else {
            auto& binder = execution.scope.resolve<binding::BodyBinder>();
            auto body = binder.bind<BodyType>(execution.context.request.body());
            res = (controller.*m_action)(body);
        }*/

        //TResult res = (controller.*m_action)(body);

        execution.context.response.status(res.statusCode());
        if (res.hasValue()) {
            execution.context.response.body(std::move(serialization::Serializer::serialize(res.value())));
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
