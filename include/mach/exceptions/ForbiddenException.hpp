#pragma once

#include <string>

#include <mach/exceptions/HttpException.hpp>

namespace mach
{
    class ForbiddenException : public HttpException {

    public:
        explicit ForbiddenException(
            std::string message = "You do not have permission to access this resource."
        );
    };
}
