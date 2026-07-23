#pragma once

#include <string>

#include <mach/Context.hpp>
#include <mach/results/Results.hpp>
#include <mach/Reply.hpp>

namespace mach
{
	namespace detail::dispatching
	{
		template <
			typename TController,
			typename TResult,
			typename... TArgs
		>
		class ControllerActionInvoker;
	}

	/**
	 * Base class for all HTTP controllers.
	 *
	 * Provides access to the current HTTP request and response through the
	 * request() and response() member functions, and exposes convenience methods
	 * for creating common HTTP responses, such as ok(), created(), badRequest(),
	 * and notFound().
	 *
	 * Controller instances are created by the framework through the dependency
	 * injection container and are intended to handle a single request.
	 *
	 * Thread safety:
	 * - Not thread-safe. Controller instances must not be shared between threads.
	 */
	class ControllerBase {

	public:
		/**
		 * Returns the current HTTP request.
		 *
		 * @return A reference to the current request.
		 *
		 * The returned reference remains valid for the lifetime of the current
		 * request being processed.
		 *
		 * @thread_safety This function is not thread-safe.
		 */
		[[nodiscard]]
		mach::Request& request() noexcept;

		/**
		 * Returns the current HTTP request.
		 *
		 * @return A const reference to the current request.
		 *
		 * The returned reference remains valid for the lifetime of the current
		 * request being processed.
		 *
		 * @thread_safety This function is not thread-safe.
		 */
		[[nodiscard]]
		const mach::Request& request() const noexcept;

		/**
		 * Returns the current HTTP response.
		 *
		 * @return A reference to the current response.
		 *
		 * The returned reference remains valid for the lifetime of the current
		 * request being processed.
		 *
		 * @thread_safety This function is not thread-safe.
		 */
		[[nodiscard]]
		mach::Response& response() noexcept;

		/**
		 * Returns the current HTTP response.
		 *
		 * @return A const reference to the current response.
		 *
		 * The returned reference remains valid for the lifetime of the current
		 * request being processed.
		 *
		 * @thread_safety This function is not thread-safe.
		 */
		[[nodiscard]]
		const mach::Response& response() const noexcept;

	protected:

		/**
		 * Creates a 200 OK response containing the specified value.
		 *
		 * @tparam T The type of the response body.
		 * @param value The response body.
		 * @return A 200 OK reply containing the specified value.
		 */
		template <typename T>
		mach::Reply<T> ok(T value);

		/**
		 * Creates a 200 OK response containing the specified string.
		 *
		 * @param value The response body.
		 * @return A 200 OK reply containing the specified string.
		 */
		mach::Reply<std::string> ok(const char* value);

		/**
		 * Creates a 201 Created response containing the specified value.
		 *
		 * @tparam T The type of the response body.
		 * @param value The response body.
		 * @return A 201 Created reply containing the specified value.
		 */
		template <typename T>
		mach::Reply<T> created(T value);

		/**
		 * Creates a 201 Created response containing the specified string.
		 *
		 * @param value The response body.
		 * @return A 201 Created reply containing the specified string.
		 */
		mach::Reply<std::string> created(const char* value);

		/**
		 * Creates a 204 No Content response.
		 *
		 * @return A 204 No Content reply.
		 */
		mach::Reply<> noContent();

		/**
		 * Creates a 400 Bad Request response.
		 *
		 * @tparam T The type of the response body. Defaults to void.
		 * @return A 400 Bad Request reply.
		 */
		template <typename T = void>
		mach::Reply<T> badRequest();

		/**
		 * Creates a 401 Unauthorized response.
		 *
		 * @tparam T The type of the response body. Defaults to void.
		 * @return A 401 Unauthorized reply.
		 */
		template <typename T = void>
		mach::Reply<T> unauthorized();

		/**
		 * Creates a 403 Forbidden response.
		 *
		 * @tparam T The type of the response body. Defaults to void.
		 * @return A 403 Forbidden reply.
		 */
		template <typename T = void>
		mach::Reply<T> forbidden();

		/**
		 * Creates a 404 Not Found response.
		 *
		 * @tparam T The type of the response body. Defaults to void.
		 * @return A 404 Not Found reply.
		 */
		template <typename T = void>
		mach::Reply<T> notFound();

		/**
		 * Creates a 409 Conflict response.
		 *
		 * @tparam T The type of the response body. Defaults to void.
		 * @return A 409 Conflict reply.
		 */
		template <typename T = void>
		mach::Reply<T> conflict();

	private:
		template <
			typename TController,
			typename TResult,
			typename... TArgs
		>
		friend class mach::detail::dispatching::ControllerActionInvoker;

		void setContext(mach::Context& context) noexcept {
			m_context = &context;
		}

		mach::Context* m_context = nullptr;
	};

	template <typename T>
	mach::Reply<T> ControllerBase::ok(T value) {
		return mach::ok<T>(value);
	}

	inline mach::Reply<std::string> ControllerBase::ok(const char* value) {
		return mach::ok(value);
	}

	template <typename T>
	mach::Reply<T> ControllerBase::created(T value) {
		return mach::created<T>(value);
	}

	inline mach::Reply<std::string> ControllerBase::created(const char* value) {
		return mach::created(value);
	}

	inline mach::Reply<> ControllerBase::noContent() {
		return mach::noContent();
	}

	template <typename T>
	mach::Reply<T> ControllerBase::badRequest() {
		return mach::badRequest<T>();
	}

	template <typename T>
	mach::Reply<T> ControllerBase::unauthorized() {
		return mach::unauthorized<T>();
	}

	template <typename T>
	mach::Reply<T> ControllerBase::forbidden() {
		return mach::forbidden<T>();
	}

	template <typename T>
	mach::Reply<T> ControllerBase::notFound() {
		return mach::notFound<T>();
	}

	template <typename T>
	mach::Reply<T> ControllerBase::conflict() {
		return mach::conflict<T>();
	}
}
