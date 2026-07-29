#pragma once

#include <string>

#include <mach/exceptions/HttpException.hpp>

namespace mach
{
    class UnauthorizedException : public HttpException {

    public:
        explicit UnauthorizedException(
            std::string message = "Authentication is required to access this resource."
        );
    };
}
