#pragma once

#include <string>

namespace mach::detail::http
{
    inline void toLowercaseInPlace(std::string& str) {
        for (char& c : str) {
            c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        }
    }
}
