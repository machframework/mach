#pragma once

#include <string_view>

#include <mach/detail/csrf/CsrfOptions.hpp>

namespace mach
{
    class AppBuilder;

    /**
     * Configures the application's Cross-Site Request Forgery (CSRF) protection.
     *
     * Provides a fluent interface for configuring the cookie and request header
     * used by the CSRF middleware.
     *
     * Instances of this class are intended to be configured through
     * `AppBuilder::addCsrf` and are not typically created directly.
     *
     * @thread_safety This class is not thread-safe.
     */
    class CsrfBuilder {

    public:
        CsrfBuilder(const CsrfBuilder&) = delete;
        CsrfBuilder& operator=(const CsrfBuilder&) = delete;

        CsrfBuilder(CsrfBuilder&&) noexcept = default;
        CsrfBuilder& operator=(CsrfBuilder&&) noexcept = default;

        ~CsrfBuilder() = default;

        /**
         * Sets the name of the cookie used to store the CSRF token.
         *
         * @param name Cookie name.
         *
         * @return A reference to this builder, allowing chaining.
         *
         * @thread_safety This function is not thread-safe.
         */
        CsrfBuilder& cookieName(std::string_view name);

        /**
         * Sets the name of the request header used to submit the CSRF token.
         *
         * @param name Header name.
         *
         * @return A reference to this builder, allowing chaining.
         *
         * @thread_safety This function is not thread-safe.
         */
        CsrfBuilder& headerName(std::string_view name);

    private:
        CsrfBuilder() = default;

        detail::csrf::CsrfOptions&& takeOptions() &&;

        detail::csrf::CsrfOptions m_options;

        friend class AppBuilder;
    };
}
