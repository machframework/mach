#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

#include <mach/http/Method.hpp>
#include <mach/http/Version.hpp>

namespace mach::detail
{
	namespace http::adapter
	{
		class BeastRequestAdapter;
	}
	namespace application
	{
		class Runtime;
	}
}

namespace mach
{
	/**
	 * Represents an HTTP request received by the server.
	 *
	 * This object provides read-only access to request metadata such as
	 * method, headers, target, and body.
	 *
	 * Lifetime:
	 * - The Request object is valid only during the request handling scope.
	 *
	 * Thread safety:
	 * - This class is not thread-safe. Concurrent access must be synchronized externally.
	 *
	 * Ownership:
	 * - The Request object is owned by the framework and should not be stored
	 *   beyond the request handling lifetime.
	 */
	class Request {

	public:

		/**
		 * Returns the HTTP method used in the request (e.g. GET, POST, PUT).
		 *
		 * @return HTTP request method.
		 *
		 * @thread_safety This function is thread-safe.
		 */
		http::Method method() const noexcept;

		/**
		 * Returns the HTTP version used in the request (e.g. 1.0, 1.1, 2.0, 3.0). 
		 * NOTE: Mach currently supports HTTP/1.0 only
		 *
		 * @return HTTP version used in the request-handling context.
		 *
		 * @thread_safety This function is thread-safe.
		 */
		http::Version version() const noexcept;

		/**
		 * Returns the targed (URL) of the request (e.g. /users).
		 *
		 * @return request target.
		 *
		 * @thread_safety This function is thread-safe.
		 */
		std::string target() const noexcept;

		/**
		 * Returns the body of the HTTP request.
		 *
		 * @return request body.
		 *
		 * @throws std::bad_alloc If memory allocation fails while returning the string.
		 *
		 * @thread_safety This function is thread-safe.
		 */
		const std::string& body() const noexcept;

		/**
		 * Returns the value of an HTTP header if it exists.
		 *
		 * @param name Header name (case-insensitive).
		 *
		 * @return A view into the stored header value, or std::nullopt if not found.
		 *
		 * @throws std::bad_alloc If memory allocation fails while returning the string.
		 *
		 * @thread_safety This function is thread-safe.
		 */
		std::optional<std::string_view> header(std::string_view name) const;

		/**
		 * Returns whether a given HTTP header exists in the request.
		 *
		 * @param name Header name (case-insensitive).
		 *
		 * @return Whether the header is found.
		 *
		 * @thread_safety This function is thread-safe.
		 */
		bool containsHeader(std::string_view name) const noexcept;

		std::string_view routeParam(std::string_view name) const;

	private:

		Request(
			http::Method method,
			http::Version version,
			std::string target,
			std::string body,
			std::unordered_map<std::string, std::string> headers
		);

		void setRouteParams(std::unordered_map<std::string, std::string>&& params);

		http::Version m_version;
		http::Method m_method;
		std::string m_target;
		std::string m_body;
		std::unordered_map<std::string, std::string> m_headers;
		std::unordered_map<std::string, std::string> m_routeParams;

		friend class detail::http::adapter::BeastRequestAdapter;
		friend class detail::application::Runtime;
	};
}
