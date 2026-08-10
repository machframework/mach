#pragma once

#include <mach/AppOptions.hpp>
#include <mach/Logger.hpp>

#include <mach/detail/di/Container.hpp>
#include <mach/detail/dispatching/Dispatcher.hpp>
#include <mach/detail/middleware/MiddlewarePipeline.hpp>
#include <mach/detail/routing/RouteEndpoint.hpp>

namespace mach::detail::application
{
    class Runtime {

    public:
        Runtime(
            const AppOptions& appOptions,
            di::Container container,
            middleware::MiddlewarePipeline middlewarePipeline,
            const Logger& logger);

        Runtime(const Runtime&) = delete;
        Runtime& operator=(const Runtime&) = delete;
        Runtime(Runtime&&) = delete;
        Runtime& operator=(Runtime&&) = delete;

        void handle(mach::Context& context) const;

    private:
        const AppOptions& m_appOptions;

        di::Container m_container;
        dispatching::Dispatcher m_dispatcher;

        const Logger& m_logger;
    };
}
