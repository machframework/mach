#pragma once

#include <string>

#include <mach/exceptions/HttpException.hpp>

namespace mach
{
    /**
     * Represents an HTTP 400 (Bad Request) exception.
     *
     * Indicates that the request could not be processed because it was invalid.
     * This exception is typically thrown when a request is syntactically correct
     * but contains invalid or inconsistent data.
     */
    class BadRequestException : public HttpException {

    public:
        /**
         * Initializes a new bad request exception.
         *
         * @param message The error message describing the bad request.
         *                Defaults to "The request was invalid.".
         */
        explicit BadRequestException(
            std::string message = "The request was invalid.");
    };
}
