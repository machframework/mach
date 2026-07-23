#pragma once

#include <stdexcept>

namespace mach::detail::exceptions
{
    class BodyBindingException : public std::runtime_error {

    public:
        using std::runtime_error::runtime_error;
    };
}
