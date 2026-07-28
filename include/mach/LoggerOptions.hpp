#pragma once

#include <mach/LogLevel.hpp>

namespace mach
{
    struct LoggerOptions {
        LogLevel level = LogLevel::Info;
        std::string pattern = "[%Y-%m-%d %H:%M:%S] [%^%l%$] %v";
        bool enableColors = true;
    };
}
