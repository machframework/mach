#pragma once

#include <string>

#include <mach/exceptions/HttpException.hpp>

namespace mach
{
    /**
     * Represents an HTTP 409 (Conflict) exception.
     *
     * Indicates that the request could not be completed because it conflicts
     * with the current state of the target resource. This exception is typically
     * thrown when attempting to create or modify a resource in a way that
     * violates application rules or resource state.
     */
    class ConflictException : public HttpException {

    public:
        /**
         * Initializes a new conflict exception.
         *
         * @param message The error message describing the conflict.
         *                Defaults to "The request could not be completed because of a resource conflict.".
         */
        explicit ConflictException(
            std::string message =
                "The request could not be completed because of a resource conflict.");
    };
}
