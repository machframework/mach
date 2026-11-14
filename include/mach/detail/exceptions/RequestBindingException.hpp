#pragma once

#include <stdexcept>

namespace mach::detail::exceptions
{
    class RequestBindingException : public std::runtime_error {

    public:
        using std::runtime_error::runtime_error;
    };
}
