#pragma once

#include <chrono>
#include <string>

namespace mach
{
    struct AppOptions {
        std::string host = "127.0.0.1";
        std::int32_t port = 3143;
        std::int64_t threadCount = 4;

        std::chrono::seconds requestTimeout = std::chrono::seconds(30);
    };
}
