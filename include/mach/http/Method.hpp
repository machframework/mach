#pragma once

#include <array>
#include <string_view>

namespace mach::http
{
    /**
     * Represents an HTTP request method.
     */
    enum class Method {
        Get,
        Post,
        Put,
        Patch,
        Delete,
        Head,
        Options,

        /// Represents an unrecognized or unsupported HTTP method.
        Unknown
    };

    /**
     * Returns the string representation of an HTTP method.
     *
     * @param method The HTTP method.
     * @return The corresponding HTTP method name.
     */
    constexpr std::string_view toString(Method method) {
        switch (method) {
        case Method::Get:
            return "GET";
        case Method::Post:
            return "POST";
        case Method::Put:
            return "PUT";
        case Method::Patch:
            return "PATCH";
        case Method::Delete:
            return "DELETE";
        case Method::Head:
            return "HEAD";
        case Method::Options:
            return "OPTIONS";
        default:
            return "UNKNOWN";
        }
    }

    /**
     * Contains all HTTP methods supported by Mach.
     */
    inline constexpr std::array<Method, 7> allMethods{
        Method::Get,
        Method::Head,
        Method::Post,
        Method::Put,
        Method::Patch,
        Method::Delete,
        Method::Options};
}
