#pragma once

#include <string>

#include <mach/exceptions/HttpException.hpp>

namespace mach
{
    /**
     * Represents an HTTP 403 (Forbidden) exception.
     *
     * Indicates that the request was understood, but the client does not have
     * permission to perform the requested operation. This exception is typically
     * thrown when authorization fails or access to a resource is denied.
     */
    class ForbiddenException final : public HttpException {

    public:
        /**
         * Initializes a new forbidden exception.
         *
         * @param message The error message describing why access was denied.
         *                Defaults to "You do not have permission to access this resource.".
         */
        explicit ForbiddenException(
            std::string message = "You do not have permission to access this resource.");
    };
}
