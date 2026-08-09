#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#include <mach/controllers/ControllerBuilder.hpp>
#include <mach/http/Method.hpp>
#include <mach/Logger.hpp>

#include <mach/AppOptions.hpp>
#include <mach/detail/controllers/ControllerTraits.hpp>
#include <mach/detail/di/Container.hpp>
#include <mach/detail/dispatching/MinimalApiInvoker.hpp>
#include <mach/detail/middleware/MiddlewarePipeline.hpp>

namespace mach
{
    class AppBuilder;

    /**
     * Represents a built Mach application.
     *
     * Used to register routes and start the underlying HTTP server after
     * application configuration has been finalized by AppBuilder.
     *
     * Ownership:
     * - Owns the application's runtime state.
     * - Should be treated as the root runtime object of a Mach application.
     *
     * Thread safety:
     * - Not thread-safe.
     *
     * Stability:
     * - This API is still experimental and may change before Mach's first stable release.
     */
    class App {

    public:
        ~App();
        App(App&&) noexcept;

        /**
         * Returns the host the application is configured to listen on (e.g. "127.0.0.1").
         *
         * @return The configured host.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] std::string host() const noexcept;

        /**
         * Returns the port the application is configured to listen on (e.g. 3143, 8080).
         *
         * @return The configured port.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] std::uint16_t port() const noexcept;

        /**
         * Returns the number of worker threads the application is configured to use.
         *
         * @return The configured thread count.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] std::size_t threadCount() const noexcept;

        /**
         * Registers a GET request handler.
         *
         * @param pattern The route pattern to match (e.g. "/api/users").
         * @param handler The function invoked when the route is matched.
         *
         * @throws std::logic_error If a route with the same method and pattern
         *         has already been registered.
         *
         * @thread_safety This function is not thread-safe.
         */
        template <typename THandler>
            requires detail::traits::MinimalApiHandler<THandler>
        void mapGet(std::string_view pattern, THandler&& handler) {
            mapRoute(http::Method::Get, pattern, std::forward<THandler>(handler));
        }

        /**
         * Registers a POST request handler.
         *
         * @param pattern The route pattern to match (e.g. "/api/users").
         * @param handler The function invoked when the route is matched.
         *
         * @throws std::logic_error If a route with the same method and pattern
         *         has already been registered.
         *
         * @thread_safety This function is not thread-safe.
         */
        template <typename THandler>
            requires detail::traits::MinimalApiHandler<THandler>
        void mapPost(std::string_view pattern, THandler&& handler) {
            mapRoute(http::Method::Post, pattern, std::forward<THandler>(handler));
        }

        /**
         * Registers a PUT request handler.
         *
         * @param pattern The route pattern to match (e.g. "/api/users").
         * @param handler The function invoked when the route is matched.
         *
         * @throws std::logic_error If a route with the same method and pattern
         *         has already been registered.
         *
         * @thread_safety This function is not thread-safe.
         */
        template <typename THandler>
            requires detail::traits::MinimalApiHandler<THandler>
        void mapPut(std::string_view pattern, THandler&& handler) {
            mapRoute(http::Method::Put, pattern, std::forward<THandler>(handler));
        }

        /**
         * Registers a PATCH request handler.
         *
         * @param pattern The route pattern to match (e.g. "/api/users").
         * @param handler The function invoked when the route is matched.
         *
         * @throws std::logic_error If a route with the same method and pattern
         *         has already been registered.
         *
         * @thread_safety This function is not thread-safe.
         */
        template <typename THandler>
            requires detail::traits::MinimalApiHandler<THandler>
        void mapPatch(std::string_view pattern, THandler&& handler) {
            mapRoute(http::Method::Patch, pattern, std::forward<THandler>(handler));
        }

        /**
         * Registers a DELETE request handler.
         *
         * @param pattern The route pattern to match (e.g. "/api/users").
         * @param handler The function invoked when the route is matched.
         *
         * @throws std::logic_error If a route with the same method and pattern
         *         has already been registered.
         *
         * @thread_safety This function is not thread-safe.
         */
        template <typename THandler>
            requires detail::traits::MinimalApiHandler<THandler>
        void mapDelete(std::string_view pattern, THandler&& handler) {
            mapRoute(http::Method::Delete, pattern, std::forward<THandler>(handler));
        }

        /**
         * Registers a HEAD request handler.
         *
         * @param pattern The route pattern to match (e.g. "/api/users").
         * @param handler The function invoked when the route is matched.
         *
         * @throws std::logic_error If a route with the same method and pattern
         *         has already been registered.
         *
         * @thread_safety This function is not thread-safe.
         */
        template <typename THandler>
            requires detail::traits::MinimalApiHandler<THandler>
        void mapHead(std::string_view pattern, THandler&& handler) {
            mapRoute(http::Method::Head, pattern, std::forward<THandler>(handler));
        }

        /**
         * Starts the application and begins accepting incoming HTTP requests.
         *
         * @return Exit status code. Returns 0 on successful shutdown, or a non-zero
         *         value if the application failed to start or encountered a fatal
         *         startup error.
         *
         * This function blocks the calling thread until the application stops.
         *
         * @thread_safety This function is not thread-safe.
         */
        [[nodiscard]] int run() const;

        /**
         * Stops the application and shuts down the HTTP server.
         *
         * If the application is currently running, this function requests a graceful
         * shutdown and causes run() to return once all shutdown operations have
         * completed.
         *
         * Calling this function before run(), after the application has already
         * stopped, or multiple times is safe and has no effect.
         *
         * @thread_safety This function is thread-safe.
         */
        void stop() const;

    private:
        App(AppOptions serverOptions,
            detail::di::Container container,
            detail::middleware::MiddlewarePipeline middlewarePipeline,
            const Logger& logger);

        template <typename THandler>
            requires detail::traits::MinimalApiHandler<THandler>
        void mapRoute(http::Method method, std::string_view pattern, THandler&& handler);

        template <detail::controllers::MachController TController>
        App& mapController();

        void addRouteImpl(detail::routing::RouteEndpoint route) const;

        void addControllerRoutesImpl(
            std::vector<detail::routing::RouteEndpoint> routes,
            std::type_index controllerType) const;

        class Impl;
        std::unique_ptr<Impl> m_impl;

        friend class AppBuilder;
    };

    template <typename THandler>
        requires detail::traits::MinimalApiHandler<THandler>
    void mach::App::mapRoute(http::Method method, std::string_view pattern, THandler&& handler) {
        using Handler = std::decay_t<THandler>;
        using Traits = detail::traits::FunctionTraits<Handler>;
        using ArgsTuple = typename Traits::ArgsTuple;
        using Result = typename Traits::ReturnType;

        using Invoker =
            detail::dispatching::MinimalApiInvokerFromTupleT<Handler, Result, ArgsTuple>;

        auto invoker = std::make_unique<Invoker>(std::forward<THandler>(handler));

        auto routeEndpoint = detail::routing::RouteEndpoint{
            .method = method,
            .pattern = std::string(pattern),
            .invoker = std::move(invoker)};

        addRouteImpl(std::move(routeEndpoint));
    }

    template <detail::controllers::MachController TController>
    App& App::mapController() {
        ControllerBuilder<TController> builder;
        TController::configure(builder);

        addControllerRoutesImpl(std::move(builder.m_controllerEndpoints), typeid(TController));
        return *this;
    }
}
