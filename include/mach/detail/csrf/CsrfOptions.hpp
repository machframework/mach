#pragma once

#include <string>

namespace mach::detail::csrf
{
    /**
     * Stores the application's CSRF protection configuration.
     *
     * Used by the CSRF middleware when validating incoming requests.
     *
     * @thread_safety
     * This type is not thread-safe during configuration. It is safe for
     * concurrent read-only access after the application is built.
     */
    struct CsrfOptions {
        std::string cookieName = "__Host-csrf-token";
        std::string headerName = "X-CSRF-Token";
    };
}
