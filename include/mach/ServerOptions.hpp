#pragma once

#include <string>
#include <string_view>

namespace mach
{
    struct ServerOptions {
        std::string host = "127.0.0.1";
        std::int32_t port = 3143;
        std::int64_t threadCount = 4;
    };
}
