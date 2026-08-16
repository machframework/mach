#include <mach/detail/dispatching/Dispatcher.hpp>

#include <mach/Context.hpp>

#include <mach/detail/di/Container.hpp>
#include <mach/detail/routing/RouteEndpoint.hpp>

namespace mach::detail::dispatching
{
    Dispatcher::Dispatcher(
        di::Container& container,
        middleware::MiddlewarePipeline&& middlewarePipeline)
        : m_container(container), m_middlewarePipeline(std::move(middlewarePipeline)) {}

    void Dispatcher::execute(mach::Context& context) const {
        auto scope = m_container.createScope();

        RequestExecution execution(context, scope);

        auto terminal = [](RequestExecution& requestExecution) {
            requestExecution.context.executionPlan.endpoint->invoker->invoke(requestExecution);
        };

        m_middlewarePipeline.invoke(execution, terminal);
    }
}
