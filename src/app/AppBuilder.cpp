#include <mach/AppBuilder.hpp>

#include <iostream>
#include <stdexcept>

#include <mach/Logger.hpp>

#include <mach/detail/binding/BodyBinder.hpp>
#include <mach/detail/exceptions/ExceptionMiddleware.hpp>
#include <mach/detail/routing/Router.hpp>
#include <mach/detail/routing/RoutingMiddleware.hpp>

namespace
{
    void validateHost(std::string_view host) {
        if (host.empty()) {
            throw std::invalid_argument("Host cannot be empty.");
        }
        if (std::ranges::any_of(host, [](unsigned char c) {
                return std::iscntrl(c) || std::isspace(c);
            })) {
            throw std::invalid_argument("Host cannot contain whitespace or control characters.");
        }
    }

    void validatePort(std::int32_t port) {
        if (port < 1 || port > 65535) {
            throw std::invalid_argument("Invalid port. Port must be between 1 and 65535.");
        }
    }

    void validateThreadCount(std::int64_t threadCount) {
        if (threadCount < 1) {
            throw std::invalid_argument("Thread count must be at least 1.");
        }
    }
}

namespace mach
{
    AppBuilder::AppBuilder() : AppBuilder(ServerOptions{}) {}

    AppBuilder::AppBuilder(ServerOptions options) {
        m_serverOptions = std::move(options);

        // register preprocessing middleware
        this->use<detail::exceptions::ExceptionMiddleware, mach::Logger>(
            mach::detail::di::ServiceAccess::Internal);
        this->use<detail::routing::RoutingMiddleware, detail::routing::Router>(
            mach::detail::di::ServiceAccess::Internal);
    }

    App AppBuilder::build() {
        mach::Logger loggerInstance;

        try {
            validateHost(m_serverOptions.host);
            validatePort(m_serverOptions.port);
            validateThreadCount(m_serverOptions.threadCount);
        } catch (const std::exception& ex) {
            loggerInstance.error("Failed to build Mach application: {}", ex.what());
            throw;
        } catch (...) {
            loggerInstance.error("Failed to build Mach application: Unknown exception");
            throw;
        }

        m_container.reserveInternal<detail::routing::Router>();

        const auto& logger = m_container.addSingletonInstance(std::move(loggerInstance), detail::di::ServiceAccess::User);

        m_container.addService<detail::binding::BodyBinder>(
            detail::di::ServiceLifetime::Singleton,
            detail::di::ServiceAccess::Internal);

        App app(
            std::move(m_serverOptions),
            std::move(m_container),
            std::move(m_middlewarePipeline),
            logger);

        for (const auto& mapper : m_controllerMappers) {
            mapper(app);
        }

        return app;
    }
}
