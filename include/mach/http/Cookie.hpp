#pragma once

#include <chrono>
#include <string>
#include <optional>

namespace mach::http
{
    /**
     * Specifies the cross - site behavior of an HTTP cookie.
     */
    enum class SameSite {
        Strict,
        Lax,
        None
    };

    /**
     * Represents an HTTP cookie.
     *
     * Stores a cookie's name, value, and optional attributes used when sending
     * cookies to clients or accessing cookies received in a request.
     *
     * Ownership:
     * - Owns all stored cookie data.
     *
     * Thread safety:
     * - Thread-safe for concurrent read-only access.
     * - Concurrent modification must be synchronized externally.
     */
    struct Cookie {
        std::string name;
        std::string value;

        std::string path = "/";
        std::optional<std::string> domain;

        std::optional<std::chrono::system_clock::time_point> expires;
        std::optional<std::chrono::seconds> maxAge;

        SameSite sameSite = SameSite::Lax;

        bool secure = false;
        bool httpOnly = false;
    };
}
