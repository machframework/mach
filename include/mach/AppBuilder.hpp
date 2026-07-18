#pragma once

#include <functional>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

#include <mach/App.hpp>

#include <mach/detail/app/ServerOptions.hpp>
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
		 * Creates a new application builder instance.
		 *
		 * @param host The network interface to bind to.
		 * @param port The port to listen on.
		 * @param threadCount The number of worker threads used to process requests.
		 *
		 * @throws std::invalid_argument If the supplied configuration is invalid.
		 */
		AppBuilder(std::string_view host, std::int32_t port = 3143, std::int64_t threadCount = 1);

		// testing 
		AppBuilder(detail::app::ServerOptions options)
			: AppBuilder(options.host, options.port, options.threads)
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
		App build();

	private:
		detail::app::ServerOptions m_serverOptions;
		detail::di::Container m_container;
		detail::middleware::MiddlewarePipeline m_middlewarePipeline;
		std::vector<std::function<void(App&)>> m_controllerMappers;

		template <typename T, typename... Deps>
		AppBuilder& use(mach::detail::di::ServiceAccess access);
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
		using Controller = std::remove_cvref_t<T>;

		constexpr bool isControllerType = mach::detail::controllers::ControllerType<Controller>;
		constexpr bool hasRouteField = mach::detail::controllers::HasPublicStaticRouteField<Controller>;
		
		static_assert(
			std::same_as<T, Controller>,
			"Mach error: controller type must not be const, volatile, or a reference"
			);

		static_assert(
			isControllerType,
			"Mach error: controller must be derived from ControllerBase"
		);

		static_assert(
			hasRouteField,
			"Mach error: controller must expose a public static route of type std::string, std::string_view, or another type convertible to std::string_view"
		);

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
		constexpr bool isMiddlewareType = mach::detail::traits::middleware::MachMiddleware<T>;
		constexpr bool isController = mach::detail::controllers::ControllerType<T>;

		static_assert(
			isMiddlewareType,
			"Mach error: middleware must expose a public method void invoke(mach::Context&, mach::Next)"
			);

		static_assert(
			!isController,
			"Mach error: middleware type must not be a controller."
			);

		if constexpr (isMiddlewareType || !isController) {
			m_container.addService<T, Deps...>(detail::di::ServiceLifetime::Scoped, access);
		}

		// add to middleware pipeline
		m_middlewarePipeline.add<T>();

		return *this;
	}
}
