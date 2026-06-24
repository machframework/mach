#pragma once

#include <memory>
#include <typeindex>

#include <mach/detail/controllers/ControllerTraits.hpp>
#include <mach/detail/results/ResultTraits.hpp>
#include <mach/detail/serailization/Serializer.hpp>

#include "IControllerActionInvoker.hpp"

namespace mach::detail::dispatching 
{
    template <
        mach::detail::controllers::MachController TController,
        mach::detail::results::ReplyResult TResult
    >
    class ControllerActionInvoker final : public IControllerActionInvoker {
    public:
        using Action = TResult (TController::*)();

        explicit ControllerActionInvoker(Action action)
            : m_action(action) {
        }

        void invoke(Context& ctx, di::Scope& scope) const override {
            auto controller = scope.resolve<TController>();
            controller->context = &ctx;

            TResult res = (controller.get()->*m_action)();

            ctx.response.status(res.statusCode());
            if (res.hasValue()) {
                ctx.response.body(std::move(serialization::Serializer::serialize(res.value())));
            }
        }

    private:
        Action m_action;
    };
}