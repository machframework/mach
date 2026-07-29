#pragma once

#include <string>

#include <mach/exceptions/HttpException.hpp>

namespace mach
{
    /**
     * Represents an HTTP 401 (Unauthorized) exception.
     *
     * Indicates that the request requires authentication or that the provided
     * authentication credentials are invalid. This exception is typically thrown
     * when a client attempts to access a protected resource without successfully
     * authenticating.
     */
    class UnauthorizedException final : public HttpException {

    public:
        /**
         * Initializes a new unauthorized exception.
         *
         * @param message The error message describing the authentication failure.
         *                Defaults to "Authentication is required to access this resource.".
         */
        explicit UnauthorizedException(
            std::string message = "Authentication is required to access this resource.");
    };
}
