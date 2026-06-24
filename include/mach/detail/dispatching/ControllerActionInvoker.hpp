#pragma once

#include <memory>
#include <typeindex>

#include <mach/detail/controllers/ControllerTraits.hpp>
#include <mach/detail/dispatching/IEndpointInvoker.hpp>
#include <mach/detail/results/ResultTraits.hpp>
#include <mach/detail/serailization/Serializer.hpp>

namespace mach::detail::dispatching 
{
    template <
        mach::detail::controllers::MachController TController,
        mach::detail::results::ReplyResult TResult
    >
    class ControllerActionInvoker final : public IEndpointInvoker {
    public:
        using Action = TResult (TController::*)();

        explicit ControllerActionInvoker(Action action)
            : m_action(action) {
        }

        void invoke(RequestExecution& execution) const override {
            auto controller = execution.scope.resolve<TController>();
            controller->context = &execution.context;

            TResult res = (controller.get()->*m_action)();

            execution.context.response.status(res.statusCode());
            if (res.hasValue()) {
                execution.context.response.body(std::move(serialization::Serializer::serialize(res.value())));
            }
        }

    private:
        Action m_action;
    };
}