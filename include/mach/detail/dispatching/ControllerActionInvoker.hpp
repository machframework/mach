#pragma once

#include <memory>
#include <typeindex>

#include <mach/detail/controllers/ControllerTraits.hpp>
#include <mach/detail/dispatching/IEndpointInvoker.hpp>
#include <mach/detail/results/ResultTraits.hpp>
#include <mach/detail/serailization/Serializer.hpp>

#include <mach/Json.hpp>

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

        explicit ControllerActionInvoker(Action action)
            : m_action(action) {
        }

        void invoke(RequestExecution& execution) const override {
            auto& controller = execution.scope.resolve<TController>();
            controller.context = &execution.context;

            // 0 - body
            // 1-N - route

            TResult res = (controller.*m_action)();

            execution.context.response.status(res.statusCode());
            if (res.hasValue()) {
                execution.context.response.body(std::move(serialization::Serializer::serialize(res.value())));
            }
        }

    private:
        Action m_action;
    };
}