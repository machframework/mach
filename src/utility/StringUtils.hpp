#pragma once


#include <cctype>
#include <string>
#include <string_view>
#include <vector>

namespace mach::detail
{
    std::vector<std::string_view> split(std::string_view str, char delimiter);
    std::string_view trim(std::string_view str);
    void toLowercaseInPlace(std::string& str);
}
