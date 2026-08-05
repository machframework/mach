#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

#include <mach/http/StatusCode.hpp>
#include <mach/http/Version.hpp>

namespace mach::http
{
    class Cookie;
}
namespace mach::detail::http::adapter
{
    class BeastRequestAdapter;
    class BeastResponseAdapter;
}
namespace mach::detail::application
{
    class Runtime;
}
namespace mach::detail::dispatching
{
    template <typename TController, typename TResult, typename... TArgs>
    class ControllerActionInvoker;

    template <typename THandler, typename TResult, typename... TArgs>
    class MinimalApiInvoker;
}
namespace mach::detail::exceptions
{
    class ExceptionMiddleware;
}

namespace mach
{
    /**
     * Represents an HTTP response produced by the server.
     *
     * This object provides access to response metadata such as
     * status code, headers, and body.
     *
     * Lifetime:
     * - The Request object is valid only during the request handling scope.
     *
     * Thread safety:
     * - This class is not thread-safe. Concurrent access must be synchronized externally.
     *
     * Ownership:
     * - The Response object is owned by the framework and should not be stored
     *   beyond the request handling lifetime.
     */
    class Response {

    public:
        /**
         * Returns the HTTP version used in the response (e.g. 1.0, 1.1, 2.0, 3.0).
         * NOTE: Mach currently supports HTTP/1.0 and HTTP/1.1 only
         *
         * @return HTTP version used in the request-handling context.
         *
         * @thread_safety This function is thread-safe.
         */
        http::Version version() const noexcept;

        /**
         * Returns the status code of the HTTP response (e.g. 200 OK, 404 Not Found).
         *
         * @return HTTP status code of the response.
         *
         * @thread_safety This function is thread-safe.
         */
        http::StatusCode status() const noexcept;

        /**
         * Returns the body of the HTTP response.
         *
         * @return response body.
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

        /**
         * Sets the HTTP status code of the response.
         *
         * @param status HTTP status code to set.
         *
         * @throws std::invalid_argument If the provided status code is not a valid HTTP status code
         * 		   supported by Mach.
         *
         * @thread_safety This function is not thread-safe.
         */
        void status(http::StatusCode status);

        /**
         * Sets the body of the response.
         *
         * @param body body to set.
         *
         * @thread_safety This function is not thread-safe.
         */
        void body(std::string body) noexcept;

        /**
         * Sets a header in the response.
         *
         * @param name header name to set.
         * @param value value to set in header.
         *
         * @throws std::invalid_argument If the header name is empty, contains
         *         characters that are not permitted in HTTP header field names,
         *         or is a reserved header (case-insensitive).
         *
         * @thread_safety This function is not thread-safe.
         */
        void setHeader(std::string_view name, std::string_view value);

    private:
        explicit Response(
            http::Version version = http::Version::Http11,
            http::StatusCode status = http::StatusCode::Ok);

        http::Version m_version;
        http::StatusCode m_status;
        std::string m_body;
        std::unordered_map<std::string, std::string> m_headers;
        std::vector<mach::http::Cookie> m_cookies;

        friend class detail::http::adapter::BeastRequestAdapter;
        friend class detail::http::adapter::BeastResponseAdapter;
        friend class detail::application::Runtime;
        friend class detail::exceptions::ExceptionMiddleware;

        template <typename TController, typename TResult, typename... TArgs>
        friend class detail::dispatching::ControllerActionInvoker;

        template <typename THandler, typename TResult, typename... TArgs>
        friend class detail::dispatching::MinimalApiInvoker;
    };
}
