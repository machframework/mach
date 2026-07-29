#pragma once

#include <string>

#include <mach/exceptions/HttpException.hpp>

namespace mach
{
    class NotFoundException : public HttpException {

    public:
        explicit NotFoundException(
            std::string message = "The requested resource was not found."
        );
    };
}
