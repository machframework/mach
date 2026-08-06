#pragma once

#include <string>

namespace mach::detail::csrf
{
    struct CsrfOptions {
        std::string cookieName = "__Host-csrf-token";
        std::string headerName = "X-CSRF-Token";
    };
}