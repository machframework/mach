#pragma once

#include <chrono>
#include <string>

namespace mach
{
    struct AppOptions {
        std::string host = "127.0.0.1";
        std::uint16_t port = 3143;
        std::size_t threadCount = 4;

        std::chrono::seconds requestTimeout = std::chrono::seconds(30);
    };
}
