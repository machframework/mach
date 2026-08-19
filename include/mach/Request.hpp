#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

#include <mach/http/Method.hpp>
#include <mach/http/Version.hpp>

#include <mach/detail/serailization/StringConversion.hpp>

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
        std::string_view target() const noexcept;

        /**
         * Returns the body of the HTTP request.
         *
         * @return request body.
         *
         * @thread_safety This function is thread-safe.
         */
        const std::string& body() const noexcept;

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
        bool containsHeader(std::string_view name) const;

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
        std::optional<std::string_view> header(std::string_view name) const;

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
        std::string_view routeParam(std::string_view name) const;

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
        T routeParam(std::string_view name) const;

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
        bool containsCookie(std::string_view name) const;

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
        std::optional<std::string_view> cookie(std::string_view name) const;

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
        bool containsQuery(std::string_view name) const;

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
        std::optional<std::string_view> query(std::string_view name) const;

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
        std::optional<T> query(std::string_view name) const;

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
