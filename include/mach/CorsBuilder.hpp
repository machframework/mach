#pragma once

#include <initializer_list>
#include <string_view>

#include <mach/http/Method.hpp>

#include <mach/detail/cors/CorsOptions.hpp>

namespace mach
{
    class AppBuilder;

    /**
     * Configures Cross-Origin Resource Sharing (CORS) policies.
     *
     * Provides a fluent interface for configuring allowed origins, methods,
     * headers, credentials, exposed headers, and other CORS settings.
     *
     * Instances of this class are intended to be configured within
     * `AppBuilder::useCors` and are not typically created directly.
     *
     * Thread safety:
     * - Not thread-safe.
     */
    class CorsBuilder {

    public:
        CorsBuilder() = default;

        CorsBuilder(const CorsBuilder&) = delete;
        CorsBuilder& operator=(const CorsBuilder&) = delete;

        CorsBuilder(CorsBuilder&&) noexcept = default;
        CorsBuilder& operator=(CorsBuilder&&) noexcept = default;

        ~CorsBuilder() = default;

        /**
         * Allows requests originating from the specified origin.
         *
         * Adds the specified origin to the set of allowed origins used during CORS
         * validation.
         *
         * @param origin The origin to allow (for example, "https://example.com").
         * @return A reference to this builder for call chaining.
         */
        CorsBuilder& allowOrigin(std::string_view origin);

        /**
         * Allows requests originating from the specified origins.
         *
         * Adds the specified origins to the set of allowed origins used during CORS
         * validation.
         *
         * @param origins The origins to allow.
         * @return A reference to this builder for call chaining.
         */
        CorsBuilder& allowOrigins(std::initializer_list<std::string_view> origins);

        /**
         * Allows requests using the specified HTTP method.
         *
         * Adds the specified HTTP method to the set of allowed methods used during
         * CORS validation.
         *
         * @param method The HTTP method to allow.
         * @return A reference to this builder for call chaining.
         */
        CorsBuilder& allowMethod(http::Method method);

        /**
         * Allows requests using the specified HTTP methods.
         *
         * Adds the specified HTTP methods to the set of allowed methods used during
         * CORS validation.
         *
         * @param methods The HTTP methods to allow.
         * @return A reference to this builder for call chaining.
         */
        CorsBuilder& allowMethods(std::initializer_list<http::Method> methods);

        /**
         * Allows the specified request header.
         *
         * Adds the specified header to the set of allowed headers used during CORS
         * validation.
         *
         * @param header The request header to allow.
         * @return A reference to this builder for call chaining.
         */
        CorsBuilder& allowHeader(std::string_view header);

        /**
         * Allows the specified request headers.
         *
         * Adds the specified headers to the set of allowed headers used during CORS
         * validation.
         *
         * @param headers The request headers to allow.
         * @return A reference to this builder for call chaining.
         */
        CorsBuilder& allowHeaders(std::initializer_list<std::string_view> headers);

        /**
         * Exposes the specified response header.
         *
         * Adds the specified header to the set of response headers exposed to clients
         * during CORS validation.
         *
         * @param header The response header to expose.
         * @return A reference to this builder for call chaining.
         */
        CorsBuilder& exposeHeader(std::string_view header);

        /**
         * Exposes the specified response headers.
         *
         * Adds the specified headers to the set of response headers exposed to clients
         * during CORS validation.
         *
         * @param headers The response headers to expose.
         * @return A reference to this builder for call chaining.
         */
        CorsBuilder& exposeHeaders(std::initializer_list<std::string_view> headers);

        /**
         * Allows requests originating from any origin.
         *
         * Configures CORS to allow requests from any origin, bypassing origin-specific
         * validation.
         *
         * @return A reference to this builder for call chaining.
         */
        CorsBuilder& allowAnyOrigin();

        /**
         * Allows any request header.
         *
         * Configures CORS to allow any request header, bypassing header-specific
         * validation.
         *
         * @return A reference to this builder for call chaining.
         */
        CorsBuilder& allowAnyHeader();

        /**
         * Allows any HTTP method.
         *
         * Configures CORS to allow any HTTP method, bypassing method-specific
         * validation.
         *
         * @return A reference to this builder for call chaining.
         */
        CorsBuilder& allowAnyMethod();

        /**
         * Enables or disables support for credentials in CORS requests.
         *
         * Configures whether browsers are allowed to include credentials, such as
         * cookies or authorization headers, in cross-origin requests.
         *
         * @param enabled Whether credentials are allowed. Defaults to `true`.
         * @return A reference to this builder for call chaining.
         */
        CorsBuilder& allowCredentials(bool enabled = true);

        /**
         * Sets the maximum age of a preflight response.
         *
         * Specifies how long browsers may cache the results of a successful CORS
         * preflight request.
         *
         * @param maxAge The maximum cache duration for preflight responses.
         * @return A reference to this builder for call chaining.
         */
        CorsBuilder& maxAge(std::chrono::seconds maxAge);

    private:
        detail::cors::CorsOptions m_options;

        detail::cors::CorsOptions&& takeOptions() &&;

        friend class AppBuilder;
    };
}
