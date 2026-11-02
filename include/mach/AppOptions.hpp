#pragma once

#include <chrono>
#include <string>

namespace mach
{
    /**
     * Represents the application's configuration.
     *
     * Stores the server configuration used when building the application.
     *
     * @thread_safety
     * This type is not thread-safe during configuration. It is safe for
     * concurrent read-only access after the application is built.
     */
    struct AppOptions {
        std::string host = "127.0.0.1";
        std::uint16_t port = 3143;
        std::size_t threadCount = 4;

        std::chrono::seconds requestTimeout = std::chrono::seconds(30);
    };
}
