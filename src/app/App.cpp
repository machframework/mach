#include <mach/App.hpp>

#include <stdexcept>
#include <string>
#include <typeindex>
#include <unordered_set>

#include <mach/detail/routing/RouteEndpoint.hpp>

#include "server/Server.hpp"

namespace
{
    enum class AppState {
        Ready,
        Running,
        Stopped
    };
}

namespace mach
{
    class App::Impl {

    public:
        Impl(
            AppOptions serverOptions,
            detail::di::Container container,
            detail::middleware::MiddlewarePipeline middlewarePipeline,
            const Logger& logger);

        ~Impl() = default;

        std::string host() const noexcept;
        std::uint16_t port() const noexcept;
        std::size_t threadCount() const noexcept;

        void mapRoute(detail::routing::RouteEndpoint route);
        void addControllerRoutes(
            std::vector<detail::routing::RouteEndpoint> routes,
            std::type_index controllerType);

        int run();
        void stop();

    private:
        AppOptions m_appOptions;
        AppState m_state = AppState::Ready;
        std::unordered_set<std::type_index> m_mappedControllers;

        std::mutex m_serverMutex;
        std::unique_ptr<detail::server::Server> m_server;

        detail::routing::Router m_router;
        detail::di::Container m_container;
        detail::middleware::MiddlewarePipeline m_middlewarePipeline;

        const Logger& m_logger;
    };

    App::App(
        AppOptions serverOptions,
        detail::di::Container container,
        detail::middleware::MiddlewarePipeline middlewarePipeline,
        const Logger& logger)
        : m_impl(
              std::make_unique<Impl>(
                  std::move(serverOptions),
                  std::move(container),
                  std::move(middlewarePipeline),
                  logger)) {}

    App::~App() = default;

    App::App(App&&) noexcept = default;

    int App::run() const {
        return m_impl->run();
    }

    void App::stop() const {
        m_impl->stop();
    }

    std::string App::host() const noexcept {
        return m_impl->host();
    }

    std::uint16_t App::port() const noexcept {
        return m_impl->port();
    }

    std::size_t App::threadCount() const noexcept {
        return m_impl->threadCount();
    }

    void App::addRouteImpl(detail::routing::RouteEndpoint route) const {
        m_impl->mapRoute(std::move(route));
    }

    void App::addControllerRoutesImpl(
        std::vector<detail::routing::RouteEndpoint> routes,
        std::type_index controllerType) const {
        m_impl->addControllerRoutes(std::move(routes), controllerType);
    }

    App::Impl::Impl(
        AppOptions serverOptions,
        detail::di::Container container,
        detail::middleware::MiddlewarePipeline middlewarePipeline,
        const Logger& logger)
        : m_appOptions(std::move(serverOptions)), m_container(std::move(container)),
          m_middlewarePipeline(std::move(middlewarePipeline)), m_logger(logger) {}

    int App::Impl::run() {
        detail::server::Server* server = nullptr;

        {
            std::lock_guard lock(m_serverMutex);

            if (m_state == AppState::Running) {
                throw std::logic_error("The application is already running");
            }

            if (m_state != AppState::Ready) {
                throw std::logic_error("The application has already run");
            }

            // This is redundant if state is authoritative.
            if (m_server) {
                throw std::logic_error("The application is already running");
            }

            m_container.addSingletonInstance<detail::routing::Router>(std::move(m_router));

            m_container.finalizeRegistrations();

            m_server = std::make_unique<detail::server::Server>(
                std::move(m_appOptions),
                std::move(m_container),
                std::move(m_middlewarePipeline),
                m_logger);

            server = m_server.get();
            m_state = AppState::Running;
        }

        int result = 0;

        try {
            server->run();
        } catch (const std::exception& exception) {
            m_logger.error("Mach error: {}", exception.what());
            result = 1;
        } catch (...) {
            m_logger.error("Mach error: unknown server failure");
            result = 1;
        }

        {
            std::lock_guard lock(m_serverMutex);

            m_server.reset();
            m_state = AppState::Stopped;
        }

        return result;
    }

    void App::Impl::stop() {
        std::lock_guard lock(m_serverMutex);

        if (m_server && m_state == AppState::Running) {
            m_server->stop();
            m_state = AppState::Stopped;
        }
    }

    void App::Impl::mapRoute(detail::routing::RouteEndpoint route) {
        if (!route.invoker) {
            throw std::invalid_argument("Route handler cannot be empty");
        }

        m_router.mapRoute(std::move(route));
    }

    void App::Impl::addControllerRoutes(
        std::vector<detail::routing::RouteEndpoint> routes,
        std::type_index controllerType) {

        if (const auto [_, inserted] = m_mappedControllers.emplace(controllerType); !inserted) {
            throw std::logic_error(
                "Mach error: controller '" + std::string(controllerType.name()) +
                "' has already been mapped");
        }

        for (auto& route : routes) {
            m_router.mapRoute(std::move(route));
        }
    }

    std::string App::Impl::host() const noexcept {
        return m_appOptions.host;
    }

    std::uint16_t App::Impl::port() const noexcept {
        return m_appOptions.port;
    }

    std::size_t App::Impl::threadCount() const noexcept {
        return m_appOptions.threadCount;
    }
}
