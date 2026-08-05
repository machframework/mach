#pragma once

#include <chrono>
#include <optional>
#include <string>
#include <unordered_set>

#include <mach/http/Method.hpp>

namespace mach::detail::cors
{
    struct CorsOptions {
        std::unordered_set<std::string> allowedOrigins;
        std::unordered_set<http::Method> allowedMethods;
        std::unordered_set<std::string> allowedHeaders;
        std::unordered_set<std::string> exposedHeaders;

        bool allowAnyOrigin = false;
        bool allowAnyMethod = false;
        bool allowAnyHeader = false;

        bool allowCredentials = false;

        std::optional<std::chrono::seconds> maxAge;
    };
}
