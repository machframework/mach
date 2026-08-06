#include "Runtime.hpp"

#include <exception>
#include <iostream>

#include <mach/http/StatusCode.hpp>

#include <mach/detail/application/ExecutionPlan.hpp>
#include <mach/detail/routing/RoutingStatus.hpp>

namespace mach::detail::application
{
    Runtime::Runtime(
        const AppOptions& appOptions,
        di::Container container,
        middleware::MiddlewarePipeline middlewarePipeline,
        const Logger& logger)
        : m_appOptions(appOptions), m_container(std::move(container)), m_logger(logger),
          m_dispatcher(m_container, std::move(middlewarePipeline)) {}

    void Runtime::handle(mach::Context& context) {
        try {
            m_dispatcher.execute(context);
        } catch (const std::exception& ex) {
            m_logger.error("Request handling failed: {}", ex.what());

            // error
            context.response = mach::Response(
                context.request.version(),
                mach::http::StatusCode::InternalServerError);
            context.response.body(
                std::string(mach::http::reasonPhrase(mach::http::StatusCode::InternalServerError)));
        } catch (...) {
            m_logger.error("Request handling failed with unknown exception");

            // error
            context.response = mach::Response(
                context.request.version(),
                mach::http::StatusCode::InternalServerError);
            context.response.body(
                std::string(mach::http::reasonPhrase(mach::http::StatusCode::InternalServerError)));
        }
    }
}
