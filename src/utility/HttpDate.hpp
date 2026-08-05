#pragma once

#include <chrono>
#include <string>

namespace mach::detail
{
    std::string formatHttpDate(std::chrono::system_clock::time_point time);
}
