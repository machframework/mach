#pragma once

#include "IControllerActionDescriptor.hpp"

#include <memory>
#include <typeindex>

namespace mach::detail::dispatching
{
    template <typename TController>
    class ControllerActionInvoker final : public IControllerActionInvoker {
    public:
        using Action = void (TController::*)(Context&);

        explicit ControllerActionInvoker(Action action)
            : m_action(action) {
        }

        void invoke(Context& ctx, di::Scope& scope) const override {
            auto controller = scope.resolve<TController>();
            (controller.get()->*m_action)(ctx);
        }

    private:
        Action m_action;
    };
}