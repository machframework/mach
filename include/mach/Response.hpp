#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

#include <mach/http/Cookie.hpp>
#include <mach/http/StatusCode.hpp>
#include <mach/http/Version.hpp>

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
        // Response properties

        /**
         * Returns the HTTP version used in the response (e.g. 1.0, 1.1, 2.0, 3.0).
         * NOTE: Mach currently supports HTTP/1.0 and HTTP/1.1 only
         *
         * @return HTTP version used in the request-handling context.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] http::Version version() const noexcept;

        /**
         * Returns the status code of the HTTP response (e.g. 200 OK, 404 Not Found).
         *
         * @return HTTP status code of the response.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] http::StatusCode status() const noexcept;

        /**
         * Returns the body of the HTTP response.
         *
         * @return response body.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] const std::string& body() const noexcept;

        /**
         * Sets the HTTP status code of the response.
         *
         * @param status HTTP status code to set.
         *
         * @throws std::invalid_argument If the provided status code is not a valid HTTP status code
         *         supported by Mach.
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

        // Generic header access

        /**
         * Returns whether a given HTTP header exists in the response.
         *
         * @param name Header name (case-insensitive).
         *
         * @return Whether the header is found.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] bool containsHeader(std::string_view name) const;

        /**
         * Returns the value of an HTTP header if it exists.
         *
         * @param name Header name (case-insensitive).
         *
         * @return A view into the stored header value, or std::nullopt if not found.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] std::optional<std::string_view> header(std::string_view name) const;

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

        // Header convenience methods

        /**
         * Returns the value of the Content-Type header.
         *
         * @return A view into the stored header value, or std::nullopt if not found.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] std::optional<std::string_view> contentType() const;

        /**
         * Returns the value of the Content-Length header.
         *
         * @return A view into the stored header value, or std::nullopt if not found.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] std::optional<std::string_view> contentLength() const;

        /**
         * Returns the value of the Content-Encoding header.
         *
         * @return A view into the stored header value, or std::nullopt if not found.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] std::optional<std::string_view> contentEncoding() const;

        /**
         * Returns the value of the Content-Language header.
         *
         * @return A view into the stored header value, or std::nullopt if not found.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] std::optional<std::string_view> contentLanguage() const;

        /**
         * Returns the value of the Content-Disposition header.
         *
         * @return A view into the stored header value, or std::nullopt if not found.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] std::optional<std::string_view> contentDisposition() const;

        /**
         * Returns the value of the Content-Range header.
         *
         * @return A view into the stored header value, or std::nullopt if not found.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] std::optional<std::string_view> contentRange() const;

        /**
         * Returns the value of the Accept-Ranges header.
         *
         * @return A view into the stored header value, or std::nullopt if not found.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] std::optional<std::string_view> acceptRanges() const;

        /**
         * Returns the value of the Cache-Control header.
         *
         * @return A view into the stored header value, or std::nullopt if not found.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] std::optional<std::string_view> cacheControl() const;

        /**
         * Returns the value of the ETag header.
         *
         * @return A view into the stored header value, or std::nullopt if not found.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] std::optional<std::string_view> etag() const;

        /**
         * Returns the value of the Last-Modified header.
         *
         * @return A view into the stored header value, or std::nullopt if not found.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] std::optional<std::string_view> lastModified() const;

        /**
         * Returns the value of the Expires header.
         *
         * @return A view into the stored header value, or std::nullopt if not found.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] std::optional<std::string_view> expires() const;

        /**
         * Returns the value of the Vary header.
         *
         * @return A view into the stored header value, or std::nullopt if not found.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] std::optional<std::string_view> vary() const;

        /**
         * Returns the value of the Age header.
         *
         * @return A view into the stored header value, or std::nullopt if not found.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] std::optional<std::string_view> age() const;

        /**
         * Returns the value of the WWW-Authenticate header.
         *
         * @return A view into the stored header value, or std::nullopt if not found.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] std::optional<std::string_view> wwwAuthenticate() const;

        /**
         * Returns the value of the Location header.
         *
         * @return A view into the stored header value, or std::nullopt if not found.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] std::optional<std::string_view> location() const;

        /**
         * Returns the value of the Retry-After header.
         *
         * @return A view into the stored header value, or std::nullopt if not found.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] std::optional<std::string_view> retryAfter() const;

        /**
         * Returns the value of the Allow header.
         *
         * @return A view into the stored header value, or std::nullopt if not found.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] std::optional<std::string_view> allow() const;

        /**
         * Returns the value of the Link header.
         *
         * @return A view into the stored header value, or std::nullopt if not found.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] std::optional<std::string_view> link() const;

        /**
         * Returns the value of the Server header.
         *
         * @return A view into the stored header value, or std::nullopt if not found.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] std::optional<std::string_view> server() const;

        /**
         * Returns the value of the Date header.
         *
         * @return A view into the stored header value, or std::nullopt if not found.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] std::optional<std::string_view> date() const;

        /**
         * Returns the value of the Content-Security-Policy header.
         *
         * @return A view into the stored header value, or std::nullopt if not found.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] std::optional<std::string_view> contentSecurityPolicy() const;

        /**
         * Returns the value of the Content-Security-Policy-Report-Only header.
         *
         * @return A view into the stored header value, or std::nullopt if not found.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] std::optional<std::string_view> contentSecurityPolicyReportOnly() const;

        /**
         * Returns the value of the Strict-Transport-Security header.
         *
         * @return A view into the stored header value, or std::nullopt if not found.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] std::optional<std::string_view> strictTransportSecurity() const;

        /**
         * Returns the value of the X-Content-Type-Options header.
         *
         * @return A view into the stored header value, or std::nullopt if not found.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] std::optional<std::string_view> xContentTypeOptions() const;

        /**
         * Returns the value of the X-Frame-Options header.
         *
         * @return A view into the stored header value, or std::nullopt if not found.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] std::optional<std::string_view> xFrameOptions() const;

        /**
         * Returns the value of the Referrer-Policy header.
         *
         * @return A view into the stored header value, or std::nullopt if not found.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] std::optional<std::string_view> referrerPolicy() const;

        /**
         * Returns the value of the Permissions-Policy header.
         *
         * @return A view into the stored header value, or std::nullopt if not found.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] std::optional<std::string_view> permissionsPolicy() const;

        // Cookies

        /**
         * Returns whether a given cookie exists in the response.
         *
         * @param name Cookie name (case-sensitive).
         *
         * @return Whether the cookie is found.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] bool containsCookie(std::string_view name) const noexcept;

        /**
         * Returns a cookie if it exists.
         *
         * @param name Cookie name (case-sensitive).
         *
         * @return The cookie if found, or std::nullopt otherwise.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] std::optional<mach::http::Cookie> cookie(std::string_view name) const;

        /**
         * Adds a cookie to the response.
         *
         * @param cookie Cookie to add.
         *
         * @thread_safety This function is not thread-safe.
         */
        void addCookie(const mach::http::Cookie& cookie);

        /**
         * Adds a cookie to the response.
         *
         * Takes ownership of the cookie when possible to avoid unnecessary copies.
         *
         * @param cookie Cookie to add.
         *
         * @thread_safety This function is not thread-safe.
         */
        void addCookie(mach::http::Cookie&& cookie);

        /**
         * Adds a cookie to the response with the specified name and value.
         *
         * @param name Cookie name.
         * @param value Cookie value.
         *
         * @thread_safety This function is not thread-safe.
         */
        void addCookie(std::string name, std::string value);

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
