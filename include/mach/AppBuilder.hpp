#pragma once

#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#include <mach/App.hpp>
#include <mach/CorsBuilder.hpp>
#include <mach/CsrfBuilder.hpp>
#include <mach/LoggerOptions.hpp>
#include <mach/AppOptions.hpp>

#include <mach/detail/controllers/ControllerTraits.hpp>
#include <mach/detail/di/Container.hpp>
#include <mach/detail/di/ServiceDescriptor.hpp>
#include <mach/detail/middleware/MiddlewarePipeline.hpp>
#include <mach/detail/middleware/MiddlewareTraits.hpp>

namespace mach
{
    /**
     * Represents the configuration entry point for building a Mach application.
     *
     * Used to register services, middleware, controllers, and other
     * application-level configuration before creating an App instance.
     *
     * Ownership:
     * - Owns the application's configuration state before build().
     * - Transfers the configured runtime state to App when build() is called.
     * - Should not be used for further configuration after build().
     *
     * Thread safety:
     * - Not thread-safe. Application configuration should be performed from one thread.
     *
     * Stability:
     * - This API is still experimental and may change before Mach's first stable release.
     */
    class AppBuilder {

    public:

        /**
         * Creates a new application builder instance using the default server configuration.
         */
        explicit AppBuilder();

        /**
         * Creates a new application builder instance.
         *
         * @param options The server configuration to use for the application.
         *
         * @throws std::invalid_argument If the supplied configuration is invalid.
         */
        explicit AppBuilder(AppOptions options);

        /**
         * Registers a scoped service in the dependency injection container.
         *
         * A scoped service is created once per scope and reused for all
         * resolutions of the same service type within that scope.
         *
         * @tparam T The service type being registered.
         * @tparam Deps The constructor dependency types required to create T.
         *
         * @return A reference to the current AppBuilder instance, allowing
         *         method chaining.
         *
         * @throws std::logic_error If the service type has already been registered.
         *
         * @thread_safety This function is not thread-safe.
         */
        template <typename T, typename... Deps>
        AppBuilder& addScoped();

        /**
         * Registers a singleton service in the dependency injection container.
         *
         * A singleton service is created once per application and is
         * reused across all resolutions of the same service type.
         *
         * @tparam T The service type being registered.
         * @tparam Deps The constructor dependency types required to create T.
         *
         * @return A reference to the current AppBuilder instance, allowing
         *         method chaining.
         *
         * @throws std::logic_error If the service type has already been registered.
         *
         * @thread_safety This function is not thread-safe.
         */
        template <typename T, typename... Deps>
        AppBuilder& addSingleton();

        /**
         * Registers a transient service in the dependency injection container.
         *
         * A transient service is created every time a resolution of its type is required.
         *
         * @tparam T The service type being registered.
         * @tparam Deps The constructor dependency types required to create T.
         *
         * @return A reference to the current AppBuilder instance, allowing
         *         method chaining.
         *
         * @throws std::logic_error If the service type has already been registered.
         *
         * @thread_safety This function is not thread-safe.
         */
        template <typename T, typename... Deps>
        AppBuilder& addTransient();

        /**
         * Registers a HTTP controller in the dependency injection container.
         *
         * Controllers will be registered as transient dependencies.
         *
         * @tparam T The service type being registered.
         * @tparam Deps The constructor dependency types required to create T.
         *
         * @return A reference to the current AppBuilder instance, allowing
         *         method chaining.
         *
         * @throws std::logic_error If the service type has already been registered.
         *
         * @thread_safety This function is not thread-safe.
         */
        template <typename T, typename... Deps>
        AppBuilder& addController();

        /**
         * Registers a middleware in the application's request pipeline.
         *
         * The middleware will be registered as a scoped dependency and executed
         * in the order it was registered. Each middleware instance is created
         * once per HTTP request.
         *
         * @tparam T The middleware type being registered.
         * @tparam Deps The constructor dependency types required to create T.
         *
         * @return A reference to the current AppBuilder instance, allowing
         *         method chaining.
         *
         * @throws std::logic_error If the middleware type has already been
         *         registered.
         *
         * @thread_safety This function is not thread-safe.
         */
        template <typename T, typename... Deps>
        AppBuilder& use();

        template <typename TConfigure>
            requires std::invocable<TConfigure, AppOptions&>
        AppBuilder& configureApp(TConfigure&& configure);

        /**
         * Configures the application's logging options.
         *
         * The provided callback is invoked immediately and receives a mutable
         * reference to the application's logger configuration. The configured
         * options are used when the application's logger is constructed during build().
         *
         * @tparam TConfigure A callable invocable with `LoggerOptions&`.
         * @param configure The callback used to configure the logger options.
         *
         * @return A reference to this builder, allowing chaining.
         *
         * @thread_safety This function is not thread-safe.
         */
        template <typename TConfigure>
            requires std::invocable<TConfigure, LoggerOptions&>
        AppBuilder& configureLogging(TConfigure&& configure);

        /**
         * Adds CORS support to the application.
         *
         * The provided callback is invoked immediately and receives a mutable
         * reference to the CORS configuration builder. The configured options are
         * used by the CORS middleware when processing requests.
         *
         * @tparam TConfigure A callable invocable with `CorsBuilder&`.
         * @param configure The callback used to configure the CORS policy.
         *
         * @return A reference to this builder, allowing chaining.
         *
         * @thread_safety This function is not thread-safe.
         */
        template <typename TConfigure>
            requires std::invocable<TConfigure, CorsBuilder&>
        AppBuilder& addCors(TConfigure&& configure);

        /**
         * Adds CSRF protection to the application.
         *
         * Adds the CSRF middleware using the default configuration.
         *
         * @return A reference to this builder, allowing chaining.
         *
         * @thread_safety This function is not thread-safe.
         */
        AppBuilder& addCsrf();

        /**
         * Adds CSRF protection to the application.
         *
         * The provided callback is invoked immediately and receives a mutable
         * reference to the CSRF configuration builder. The configured options are
         * used by the CSRF middleware when processing requests.
         *
         * @tparam TConfigure A callable invocable with `CsrfBuilder&`.
         * @param configure The callback used to configure CSRF protection.
         *
         * @return A reference to this builder, allowing chaining.
         *
         * @thread_safety This function is not thread-safe.
         */
        template <typename TConfigure>
            requires std::invocable<TConfigure, CsrfBuilder&>
        AppBuilder& addCsrf(TConfigure&& configure);

        /**
         * Builds and returns the application instance.
         *
         * Finalizes the application configuration, including all registered
         * services, middleware, controllers, and transfers
         * ownership of the configured state to the returned App.
         *
         * After calling this function, the AppBuilder should not be used
         * to perform further configuration.
         *
         * @return A fully configured App instance.
         *
         * @throws std::logic_error If the application configuration is invalid
         *         or incomplete.
         *
         * @thread_safety This function is not thread-safe.
         */
        App build();

    private:
        detail::di::Container m_container;
        detail::middleware::MiddlewarePipeline m_middlewarePipeline;
        std::vector<std::function<void(App&)>> m_controllerMappers;

        AppOptions m_appOptions;
        LoggerOptions m_loggerOptions;
        std::optional<detail::cors::CorsOptions> m_corsOptions;
        std::optional<detail::csrf::CsrfOptions> m_csrfOptions;

        template <typename T, typename... Deps>
        AppBuilder& use(mach::detail::di::ServiceAccess access);
    };

    template <typename T, typename... Deps>
    AppBuilder& AppBuilder::addScoped() {
        static_assert(
            !mach::detail::controllers::ValidController<T>,
            "Mach error: Controllers must be registered using addController<T>(), not "
            "addScoped<T>().");

        static_assert(
            (!mach::detail::controllers::ValidController<Deps> && ...),
            "Mach error: services must not depend on controllers.");

        m_container.addService<T, Deps...>(detail::di::ServiceLifetime::Scoped);
        return *this;
    }

    template <typename T, typename... Deps>
    AppBuilder& AppBuilder::addSingleton() {
        static_assert(
            !mach::detail::controllers::ValidController<T>,
            "Mach error: controllers must be registered using addController<T>(), not "
            "addSingleton<T>().");

        m_container.addService<T, Deps...>(detail::di::ServiceLifetime::Singleton);
        return *this;
    }

    template <typename T, typename... Deps>
    AppBuilder& AppBuilder::addTransient() {
        static_assert(
            !mach::detail::controllers::ValidController<T>,
            "Mach error: controllers must be registered using addController<T>(), not "
            "addTransient<T>().");

        m_container.addService<T, Deps...>(detail::di::ServiceLifetime::Transient);
        return *this;
    }

    template <typename T, typename... Deps>
    AppBuilder& AppBuilder::addController() {
        using Controller = std::remove_cvref_t<T>;

        constexpr bool isControllerType = mach::detail::controllers::ControllerType<Controller>;
        constexpr bool hasRouteField =
            mach::detail::controllers::HasPublicStaticRouteField<Controller>;

        static_assert(
            std::same_as<T, Controller>,
            "Mach error: controller type must not be const, volatile, or a reference");

        static_assert(
            isControllerType,
            "Mach error: controller must be derived from ControllerBase");

        static_assert(
            hasRouteField,
            "Mach error: controller must expose a public static route of type std::string, "
            "std::string_view, or another type convertible to std::string_view");

        if constexpr (isControllerType && hasRouteField) {
            m_container.addService<T, Deps...>(detail::di::ServiceLifetime::Transient);

            m_controllerMappers.emplace_back([](App& app) {
                app.mapController<T>();
            });
        }

        return *this;
    }

    template <typename T, typename... Deps>
    AppBuilder& AppBuilder::use() {
        return this->use<T, Deps...>(mach::detail::di::ServiceAccess::User);
    }

    template <typename T, typename... Deps>
    AppBuilder& AppBuilder::use(mach::detail::di::ServiceAccess access) {
        constexpr bool isValidMiddlewareType = detail::traits::middleware::ValidMiddlewareType<T>;
        constexpr bool hasValidMiddlewareInvoke =
            detail::traits::middleware::HasValidMiddlewareInvoke<T>;
        constexpr bool isController = mach::detail::controllers::ControllerType<T>;

        static_assert(
            isValidMiddlewareType,
            "Mach error: middleware must be a non-cv, non-reference class type.");

        static_assert(
            hasValidMiddlewareInvoke,
            "Mach error: middleware must expose "
            "'void invoke(mach::Context&, const mach::Next&)'.");

        static_assert(!isController, "Mach error: middleware type must not be a controller.");

        if constexpr (isValidMiddlewareType && hasValidMiddlewareInvoke && !isController) {
            m_container.addService<T, Deps...>(detail::di::ServiceLifetime::Scoped, access);
        }

        m_middlewarePipeline.add<T>();
        return *this;
    }

    template <typename TConfigure>
        requires std::invocable<TConfigure, AppOptions&>
    AppBuilder& AppBuilder::configureApp(TConfigure&& configure) {
        std::invoke(std::forward<TConfigure>(configure), m_appOptions);
        return *this;
    }

    template <typename TConfigure>
        requires std::invocable<TConfigure, LoggerOptions&>
    AppBuilder& AppBuilder::configureLogging(TConfigure&& configure) {
        std::invoke(std::forward<TConfigure>(configure), m_loggerOptions);
        return *this;
    }

    template <typename TConfigure>
        requires std::invocable<TConfigure, CorsBuilder&>
    AppBuilder& AppBuilder::addCors(TConfigure&& configure) {
        CorsBuilder builder;
        std::invoke(std::forward<TConfigure>(configure), builder);
        m_corsOptions = std::move(builder).takeOptions();
        return *this;
    }

    template <typename TConfigure>
        requires std::invocable<TConfigure, CsrfBuilder&>
    AppBuilder& AppBuilder::addCsrf(TConfigure&& configure) {
        CsrfBuilder builder;
        std::invoke(std::forward<TConfigure>(configure), builder);
        m_csrfOptions = std::move(builder).takeOptions();

        return *this;
    }
}
