#include <mach/detail/dispatching/Dispatcher.hpp>

#include <mach/Context.hpp>

#include <mach/detail/routing/RouteEndpoint.hpp>
#include <mach/detail/di/Container.hpp>

namespace mach::detail::dispatching
{
    Dispatcher::Dispatcher(
        di::Container& container,
        middleware::MiddlewarePipeline&& middlewarePipeline)
        : m_container(container), m_middlewarePipeline(std::move(middlewarePipeline)) {}

    void Dispatcher::execute(mach::Context& context) const {
        auto scope = m_container.createScope();

        RequestExecution execution(context, scope);

        auto terminal = [](RequestExecution& execution) {
            execution.context.executionPlan.endpoint->invoker->invoke(execution);
        };

        m_middlewarePipeline.invoke(execution, terminal);
    }
}
