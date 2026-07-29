#pragma once

#include <string>

#include <mach/exceptions/HttpException.hpp>

namespace mach
{
    class BadRequestException : public HttpException {

    public:
        explicit BadRequestException(
            std::string message = std::string message = "The request was invalid.");
    };
}
