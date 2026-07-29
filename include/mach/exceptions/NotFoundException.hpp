#pragma once

#include <string>

#include <mach/exceptions/HttpException.hpp>

namespace mach
{
    /**
     * Represents an HTTP 404 (Not Found) exception.
     *
     * Indicates that the requested resource could not be found. This exception
     * is typically thrown when a requested entity or endpoint does not exist.
     */
    class NotFoundException final : public HttpException {

    public:
        /**
         * Initializes a new not found exception.
         *
         * @param message The error message describing the missing resource.
         *                Defaults to "The requested resource was not found.".
         */
        explicit NotFoundException(std::string message = "The requested resource was not found.");
    };
}
