#pragma once

#include <string>

#include <mach/exceptions/HttpException.hpp>

namespace mach
{
    class ConflictException : public HttpException {

    public:
        explicit ConflictException(
            std::string message = "The request could not be completed because of a resource conflict."
        );
    };
}
