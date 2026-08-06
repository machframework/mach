#include <mach/AppBuilder.hpp>

#include <iostream>
#include <stdexcept>

#include <mach/Logger.hpp>

#include <mach/detail/binding/BodyBinder.hpp>
#include <mach/detail/cors/CorsOptions.hpp>
#include <mach/detail/exceptions/ExceptionMiddleware.hpp>
#include <mach/detail/routing/Router.hpp>
#include <mach/detail/routing/RoutingMiddleware.hpp>

#include "cors/CorsMiddleware.hpp"
#include "csrf/CsrfMiddleware.hpp"

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
    AppBuilder::AppBuilder() : AppBuilder(AppOptions{}) {}

    AppBuilder::AppBuilder(AppOptions options) {
        m_appOptions = std::move(options);

        // reserve preprocessing middleware
        m_container.reserveInternal<detail::cors::CorsMiddleware>();
        m_container.reserveInternal<detail::csrf::CsrfMiddleware>();

        m_container.reserveInternal<detail::routing::RoutingMiddleware>();
        m_container.reserveInternal<detail::routing::Router>();

        this->use<detail::exceptions::ExceptionMiddleware, mach::Logger>(
            mach::detail::di::ServiceAccess::Internal);
    }

    AppBuilder& AppBuilder::addCsrf() {
        if (!m_csrfOptions) {
            m_csrfOptions = detail::csrf::CsrfOptions{};
        }

        return *this;
    }

    App AppBuilder::build() {
        mach::Logger loggerInstance(std::move(m_loggerOptions));

        try {
            validateHost(m_appOptions.host);
            validatePort(m_appOptions.port);
            validateThreadCount(m_appOptions.threadCount);
        } catch (const std::exception& ex) {
            loggerInstance.error("Failed to build Mach application: {}", ex.what());
            throw;
        } catch (...) {
            loggerInstance.error("Failed to build Mach application: Unknown exception");
            throw;
        }

        const auto& logger = m_container.addSingletonInstance(std::move(loggerInstance), detail::di::ServiceAccess::User);

        m_container.addService<detail::binding::BodyBinder>(
            detail::di::ServiceLifetime::Singleton,
            detail::di::ServiceAccess::Internal);

        if (m_corsOptions) {
            m_container.addSingletonInstance<detail::cors::CorsOptions>(
                std::move(*m_corsOptions),
                detail::di::ServiceAccess::Internal);

            this->use<detail::cors::CorsMiddleware, detail::cors::CorsOptions>(
                mach::detail::di::ServiceAccess::Internal);
        }
        if (m_csrfOptions) {
            m_container.addSingletonInstance<detail::csrf::CsrfOptions>(
                std::move(*m_csrfOptions),
                detail::di::ServiceAccess::Internal);

            this->use<detail::csrf::CsrfMiddleware, detail::csrf::CsrfOptions>(
                mach::detail::di::ServiceAccess::Internal);
        }

        this->use<detail::routing::RoutingMiddleware, detail::routing::Router>(
            mach::detail::di::ServiceAccess::Internal);

        App app(
            std::move(m_appOptions),
            std::move(m_container),
            std::move(m_middlewarePipeline),
            logger);

        // map controllers
        for (const auto& mapper : m_controllerMappers) {
            mapper(app);
        }

        return app;
    }
}
