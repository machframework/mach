#pragma once

#include <chrono>
#include <string>
#include <optional>

namespace mach::http
{
    enum class SameSite {
        Strict,
        Lax,
        None
    };

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
