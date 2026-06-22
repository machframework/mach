#pragma once

#include <memory>
#include <string>
#include <string_view>

#include <mach/Context.hpp>
#include <mach/http/Method.hpp>

#include <mach/detail/app/ServerOptions.hpp>
#include <mach/detail/di/Container.hpp>
#include <mach/detail/core/MinimalApiHandler.hpp>

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

		/**
		 * Returns the host the application is configured to listen on (e.g. "127.0.0.1").
		 *
		 * @return The configured host.
		 *
		 * @thread_safety This function is thread-safe.
		 */
		std::string host() const noexcept;

		/**
		 * Returns the port the application is configured to listen on (e.g. 3143, 8080).
		 *
		 * @return The configured port.
		 *
		 * @thread_safety This function is thread-safe.
		 */
		std::uint16_t port() const noexcept;

		/**
		 * Returns the number of worker threads the application is configured to use.
		 *
		 * @return The configured thread count.
		 *
		 * @thread_safety This function is thread-safe.
		 */
		std::size_t threadCount() const noexcept;

		/**
		 * Registers a GET request handler.
		 *
		 * @param pattern The route pattern to match (e.g. "/api/users").
		 * @param handler The function invoked when the route is matched.
		 *
		 * @throws std::invalid_argument If the supplied handler is invalid.
		 * @throws std::logic_error If a route with the same method and pattern
		 *         has already been registered.
		 *
		 * @thread_safety This function is not thread-safe.
		 */
		template <typename THandler>
		requires std::is_invocable_r_v<void, THandler, mach::Context&>
		void get(std::string_view pattern, THandler&& handler) {
			addRoute(
				http::Method::Get,
				pattern,
				std::forward<THandler>(handler)
			);
		}

		/**
		 * Registers a POST request handler.
		 *
		 * @param pattern The route pattern to match (e.g. "/api/users").
		 * @param handler The function invoked when the route is matched.
		 *
		 * @throws std::invalid_argument If the supplied handler is invalid.
		 * @throws std::logic_error If a route with the same method and pattern
		 *         has already been registered.
		 *
		 * @thread_safety This function is not thread-safe.
		 */
		template <typename THandler>
		requires std::is_invocable_r_v<void, THandler, mach::Context&>
		void post(std::string_view pattern, THandler&& handler) {
			addRoute(
				http::Method::Post,
				pattern,
				std::forward<THandler>(handler)
			);
		}

		/**
		 * Registers a PUT request handler.
		 *
		 * @param pattern The route pattern to match (e.g. "/api/users").
		 * @param handler The function invoked when the route is matched.
		 *
		 * @throws std::invalid_argument If the supplied handler is invalid.
		 * @throws std::logic_error If a route with the same method and pattern
		 *         has already been registered.
		 *
		 * @thread_safety This function is not thread-safe.
		 */
		template <typename THandler>
		requires std::is_invocable_r_v<void, THandler, mach::Context&>
		void put(std::string_view pattern, THandler&& handler) {
			addRoute(
				http::Method::Put,
				pattern,
				std::forward<THandler>(handler)
			);
		}

		/**
		 * Registers a PATCH request handler.
		 *
		 * @param pattern The route pattern to match (e.g. "/api/users").
		 * @param handler The function invoked when the route is matched.
		 *
		 * @throws std::invalid_argument If the supplied handler is invalid.
		 * @throws std::logic_error If a route with the same method and pattern
		 *         has already been registered.
		 *
		 * @thread_safety This function is not thread-safe.
		 */
		template <typename THandler>
		requires std::is_invocable_r_v<void, THandler, mach::Context&>
		void patch(std::string_view pattern, THandler&& handler) {
			addRoute(
				http::Method::Patch,
				pattern,
				std::forward<THandler>(handler)
			);
		}

		/**
		 * Registers a DELETE request handler.
		 *
		 * @param pattern The route pattern to match (e.g. "/api/users").
		 * @param handler The function invoked when the route is matched.
		 *
		 * @throws std::invalid_argument If the supplied handler is invalid.
		 * @throws std::logic_error If a route with the same method and pattern
		 *         has already been registered.
		 *
		 * @thread_safety This function is not thread-safe.
		 */
		template <typename THandler>
		requires std::is_invocable_r_v<void, THandler, mach::Context&>
		void del(std::string_view pattern, THandler&& handler) {
			addRoute(
				http::Method::Delete,
				pattern,
				std::forward<THandler>(handler)
			);
		}

		/**
		 * Registers a route handler.
		 *
		 * @param method The HTTP method to match (e.g. GET, POST).
		 * @param pattern The route pattern to match (e.g. "/api/users").
		 * @param handler The function invoked when the route is matched.
		 *
		 * @throws std::invalid_argument If the supplied handler is invalid.
		 * @throws std::logic_error If a route with the same method and pattern
		 *         has already been registered.
		 *
		 * @thread_safety This function is not thread-safe.
		 */
		template <typename THandler>
		requires std::is_invocable_r_v<void, THandler, mach::Context&>
		void addRoute(http::Method method, std::string_view pattern, THandler&& handler) {
			addRouteImpl(
				method,
				pattern,
				detail::MinimalApiHandler{ std::forward<THandler>(handler) }
			);
		}

		/**
		 * Starts the application and begins accepting incoming HTTP requests.
		 *
		 * This function blocks the calling thread until the application stops.
		 *
		 * @thread_safety This function is not thread-safe.
		 */
		void run();

	private:
	
		App(detail::app::ServerOptions serverOptions, detail::di::Container container);
		void addRouteImpl(http::Method method, std::string_view pattern, detail::MinimalApiHandler handler);

		class Impl;
		std::unique_ptr<Impl> m_impl;

		friend class AppBuilder;
	};
}
