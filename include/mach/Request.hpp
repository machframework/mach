#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

#include <mach/http/Method.hpp>
#include <mach/http/Version.hpp>

#include <mach/detail/serialization/StringConversion.hpp>

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
    namespace routing
    {
        class Router;
        class RoutingMiddleware;
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
        // Request properties

        /**
         * Returns the HTTP method used in the request (e.g. GET, POST, PUT).
         *
         * @return HTTP request method.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] http::Method method() const noexcept;

        /**
         * Returns the HTTP version used in the request (e.g. 1.0, 1.1, 2.0, 3.0).
         * NOTE: Mach currently supports HTTP/1.0 only
         *
         * @return HTTP version used in the request-handling context.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] http::Version version() const noexcept;

        /**
         * Returns the targed (URL) of the request (e.g. /users).
         *
         * @return request target.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] std::string_view target() const noexcept;

        /**
         * Returns the body of the HTTP request.
         *
         * @return request body.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] const std::string& body() const noexcept;


        // Generic header access

        /**
         * Returns whether a given HTTP header exists in the request.
         *
         * @param name Header name (case-insensitive).
         *
         * @return Whether the header is found.
         * 
         * @throws std::bad_alloc If memory allocation fails.
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
         * @throws std::bad_alloc If memory allocation fails.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] std::optional<std::string_view> header(std::string_view name) const;


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
         * Returns the value of the Accept header.
         *
         * @return A view into the stored header value, or std::nullopt if not found.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] std::optional<std::string_view> accept() const;

        /**
         * Returns the value of the Accept-Encoding header.
         *
         * @return A view into the stored header value, or std::nullopt if not found.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] std::optional<std::string_view> acceptEncoding() const;

        /**
         * Returns the value of the Accept-Language header.
         *
         * @return A view into the stored header value, or std::nullopt if not found.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] std::optional<std::string_view> acceptLanguage() const;

        /**
         * Returns the value of the Authorization header.
         *
         * @return A view into the stored header value, or std::nullopt if not found.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] std::optional<std::string_view> authorization() const;

        /**
         * Returns the value of the Host header.
         *
         * @return A view into the stored header value, or std::nullopt if not found.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] std::optional<std::string_view> host() const;

        /**
         * Returns the value of the User-Agent header.
         *
         * @return A view into the stored header value, or std::nullopt if not found.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] std::optional<std::string_view> userAgent() const;

        /**
         * Returns the value of the Origin header.
         *
         * @return A view into the stored header value, or std::nullopt if not found.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] std::optional<std::string_view> origin() const;

        /**
         * Returns the value of the Referer header.
         *
         * @return A view into the stored header value, or std::nullopt if not found.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] std::optional<std::string_view> referer() const;

        /**
         * Returns the value of the Cache-Control header.
         *
         * @return A view into the stored header value, or std::nullopt if not found.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] std::optional<std::string_view> cacheControl() const;

        /**
         * Returns the value of the If-Match header.
         *
         * @return A view into the stored header value, or std::nullopt if not found.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] std::optional<std::string_view> ifMatch() const;

        /**
         * Returns the value of the If-None-Match header.
         *
         * @return A view into the stored header value, or std::nullopt if not found.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] std::optional<std::string_view> ifNoneMatch() const;

        /**
         * Returns the value of the If-Modified-Since header.
         *
         * @return A view into the stored header value, or std::nullopt if not found.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] std::optional<std::string_view> ifModifiedSince() const;

        /**
         * Returns the value of the If-Unmodified-Since header.
         *
         * @return A view into the stored header value, or std::nullopt if not found.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] std::optional<std::string_view> ifUnmodifiedSince() const;

        /**
         * Returns the value of the Range header.
         *
         * @return A view into the stored header value, or std::nullopt if not found.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] std::optional<std::string_view> range() const;

        /**
         * Returns the value of the If-Range header.
         *
         * @return A view into the stored header value, or std::nullopt if not found.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] std::optional<std::string_view> ifRange() const;

        /**
         * Returns the value of the Forwarded header.
         *
         * @return A view into the stored header value, or std::nullopt if not found.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] std::optional<std::string_view> forwarded() const;

        /**
         * Returns the value of the X-Forwarded-For header.
         *
         * @return A view into the stored header value, or std::nullopt if not found.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] std::optional<std::string_view> xForwardedFor() const;

        /**
         * Returns the value of the X-Forwarded-Host header.
         *
         * @return A view into the stored header value, or std::nullopt if not found.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] std::optional<std::string_view> xForwardedHost() const;

        /**
         * Returns the value of the X-Forwarded-Proto header.
         *
         * @return A view into the stored header value, or std::nullopt if not found.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] std::optional<std::string_view> xForwardedProto() const;

        /**
         * Returns the media type specified by the Content-Type header.
         *
         * @return A view into the media type without parameters, or std::nullopt if absent or
         * invalid.
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] std::optional<std::string_view> mediaType() const;

        /**
         * Returns the charset specified by the Content-Type header.
         *
         * @return A view into the charset without surrounding quotes, or std::nullopt if absent or
         * invalid.
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] std::optional<std::string_view> charset() const;

        /**
         * Checks whether the media type matches the specified media type.
         *
         * @param type The media type to match (case-insensitive, without parameters).
         * @return True if the media types match, otherwise false.
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] bool isContentType(std::string_view type) const;

        /**
         * Checks whether the Content-Type header specifies a JSON media type.
         *
         * @return True if the media type is application/json or has a +json subtype suffix,
         * otherwise false.
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] bool isJson() const;

        /**
         * Checks whether the Content-Type header specifies multipart/form-data.
         *
         * @return True if the media type matches multipart/form-data, otherwise false.
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] bool isMultipartFormData() const;

        /**
         * Checks whether the Content-Type header specifies application/x-www-form-urlencoded.
         *
         * @return True if the media type matches application/x-www-form-urlencoded, otherwise
         * false.
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] bool isFormUrlEncoded() const;

        /**
         * Returns the Content-Length header value in bytes.
         *
         * @return The content length in bytes, or std::nullopt if absent or invalid.
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] std::optional<std::uint64_t> contentLengthBytes() const;


        // Route parameters

        /**
         * Returns the value of a route parameter.
         *
         * @param name Parameter name (case-sensitive).
         *
         * @return A view into the stored parameter value.
         *
         * @throws std::bad_alloc If memory allocation fails.
         * @throws std::out_of_range If the parameter does not exist.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] std::string_view routeParam(std::string_view name) const;

        /**
         * Returns the value of a route parameter converted to the specified type.
         *
         * @tparam T Type to convert the route parameter value to.
         *
         * @param name Route parameter name (case-sensitive).
         *
         * @return The route parameter value converted to T.
         *
         * @throws std::bad_alloc If memory allocation fails during conversion.
         * @throws std::out_of_range If the route parameter does not exist.
         * @throws std::invalid_argument If the route parameter value cannot be converted to T.
         *
         * @thread_safety This function is thread-safe.
         */
        template <typename T>
        [[nodiscard]] T routeParam(std::string_view name) const;


        // Cookies

        /**
         * Returns whether a given cookie exists in the request.
         *
         * @param name Cookie name (case-sensitive).
         *
         * @return Whether the cookie is found.
         * 
         * @throws std::bad_alloc If memory allocation fails.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] bool containsCookie(std::string_view name) const;

        /**
         * Returns the value of an HTTP cookie if it exists.
         *
         * @param name Cookie name (case-sensitive).
         *
         * @return A view into the stored cookie value, or std::nullopt if not found.
         *
         * @throws std::bad_alloc If memory allocation fails.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] std::optional<std::string_view> cookie(std::string_view name) const;


        // Query parameters

        /**
         * Returns whether a given query parameter exists in the request.
         *
         * @param name Query parameter name (case-sensitive).
         *
         * @return Whether the query parameter is found.
         * 
         * @throws std::bad_alloc If memory allocation fails during lookup.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] bool containsQuery(std::string_view name) const;

        /**
         * Returns the value of a query parameter.
         *
         * @param name Query parameter name (case-sensitive).
         *
         * @return A view into the stored query parameter value, or std::nullopt if the
         * query parameter does not exist.
         *
         * @throws std::bad_alloc If memory allocation fails during lookup.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]] std::optional<std::string_view> query(std::string_view name) const;

        /**
         * Returns the value of a query parameter converted to the specified type.
         *
         * @tparam T Type to convert the query parameter value to.
         *
         * @param name Query parameter name (case-sensitive).
         *
         * @return The query parameter value converted to T, or std::nullopt if the
         * query parameter does not exist or cannot be converted to T.
         *
         * @throws std::bad_alloc If memory allocation fails during lookup.
         *
         * @thread_safety This function is thread-safe.
         */
        template <typename T>
        [[nodiscard]] std::optional<T> query(std::string_view name) const;

    private:
        Request(
            http::Method method,
            http::Version version,
            std::string target,
            std::string body,
            std::unordered_map<std::string, std::string> headers,
            std::unordered_map<std::string, std::string> cookies);

        void setRouteParams(std::unordered_map<std::string, std::string>&& params);
        void setRouteQuery(std::unordered_map<std::string, std::string>&& query);

        http::Version m_version;
        http::Method m_method;
        std::string m_target;
        std::string m_body;
        std::unordered_map<std::string, std::string> m_headers;
        std::unordered_map<std::string, std::string> m_routeParams;
        std::unordered_map<std::string, std::string> m_query;
        std::unordered_map<std::string, std::string> m_cookies;

        friend class detail::http::adapter::BeastRequestAdapter;
        friend class detail::application::Runtime;
        friend class detail::routing::Router;
        friend class detail::routing::RoutingMiddleware;
    };

    template <typename T>
    T Request::routeParam(std::string_view name) const {
        return mach::fromString<T>(this->routeParam(name));
    }

    template <typename T>
    std::optional<T> Request::query(std::string_view name) const {
        if (const auto result = this->query(name)) {
            try {
                return mach::fromString<T>(*result);
            } catch (const std::invalid_argument&) {
                return std::nullopt;
            } catch (const std::out_of_range&) {
                return std::nullopt;
            }
        }

        return std::nullopt;
    }
}
