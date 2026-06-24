#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

#include <mach/App.hpp>

#include <mach/detail/app/ServerOptions.hpp>
#include <mach/detail/di/Container.hpp>
#include <mach/detail/controllers/ControllerTraits.hpp>
#include <mach/detail/middleware/MiddlewareTraits.hpp>
#include <mach/detail/middleware/MiddlewareInvoker.hpp>

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
		 * Creates a new application builder instance.
		 *
		 * @param host The network interface to bind to.
		 * @param port The port to listen on.
		 * @param threadCount The number of worker threads used to process requests.
		 *
		 * @throws std::invalid_argument If the supplied configuration is invalid.
		 */
		AppBuilder(std::string_view host, std::uint16_t port, std::size_t threadCount = 1);

		// testing 
		AppBuilder(detail::app::ServerOptions options)
			: m_serverOptions(std::move(options))
		{ }
		// testing

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

		template <typename T, typename... Deps>
		AppBuilder& use();

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
		App build() const;

	private:
		detail::app::ServerOptions m_serverOptions;
		detail::di::Container m_container;
		std::vector<std::unique_ptr<detail::middleware::IMiddlewareInvoker>> m_middleware;
	};

	template <typename T, typename... Deps>
	AppBuilder& AppBuilder::addScoped() {
		static_assert(
			!mach::detail::controllers::ValidController<T>,
			"Mach error: Controllers must be registered using addController<T>(), not addScoped<T>()."
		);

		static_assert(
			(!mach::detail::controllers::ValidController<Deps> && ...),
			"Mach error: services must not depend on controllers."
		);

		m_container.addService<T, Deps...>(detail::di::ServiceLifetime::Scoped);
		return *this;
	}

	template <typename T, typename... Deps>
	AppBuilder& AppBuilder::addSingleton() {
		static_assert(
			!mach::detail::controllers::ValidController<T>,
			"Mach error: controllers must be registered using addController<T>(), not addSingleton<T>()."
		);

		m_container.addService<T, Deps...>(detail::di::ServiceLifetime::Singleton);
		return *this;
	}

	template <typename T, typename... Deps>
	AppBuilder& AppBuilder::addTransient() {
		static_assert(
			!mach::detail::controllers::ValidController<T>,
			"Mach error: controllers must be registered using addController<T>(), not addTransient<T>()."
		);

		m_container.addService<T, Deps...>(detail::di::ServiceLifetime::Transient);
		return *this;
	}

	template <typename T, typename... Deps>
	AppBuilder& AppBuilder::addController() {
		constexpr bool isControllerType = mach::detail::controllers::ControllerType<T>;
		constexpr bool hasRouteField = mach::detail::controllers::HasPublicStaticRouteField<T>;
		
		static_assert(
			isControllerType,
			"Mach error: controller must be derived from ControllerBase"
		);

		static_assert(
			hasRouteField,
			"Mach error: controller must expose a public std::string route field"
		);

		if constexpr (isControllerType && hasRouteField) {
			m_container.addService<T, Deps...>(detail::di::ServiceLifetime::Transient);
		}

		return *this;
	}

	template <typename T, typename... Deps>
	AppBuilder& AppBuilder::use() {
		constexpr bool isMiddlewareType = mach::detail::middleware::MachMiddleware<T>;

		static_assert(
			isMiddlewareType,
			"Mach error: middleware must expose a public method void invoke(mach::Context&, mach::Next)"
		);

		// validate deps

		if constexpr (isMiddlewareType) {
			m_container.addService<T, Deps...>(detail::di::ServiceLifetime::Scoped);
		}

		// add to middleware pipeline
		auto invoker = std::make_unique<detail::middleware::MiddlewareInvoker<T>>();
		m_middleware.emplace_back(std::move(invoker));

		return *this;
	}
}
