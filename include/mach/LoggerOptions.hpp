#pragma once

#include <mach/LogLevel.hpp>

namespace mach
{
    /**
     * Represents the application's logging configuration.
     *
     * Stores the configuration used when constructing the application's logger.
     *
     * @thread_safety
     * This type is not thread-safe during configuration. It is safe for
     * concurrent read-only access after the application is built.
     */
    struct LoggerOptions {
        LogLevel level = LogLevel::Info;
        std::string pattern = "[%Y-%m-%d %H:%M:%S] [%^%l%$] %v";
        bool enableColors = true;
    };
}
