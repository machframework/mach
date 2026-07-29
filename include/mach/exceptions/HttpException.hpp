#pragma once

#include <stdexcept>
#include <string>

#include <mach/http/StatusCode.hpp>

namespace mach
{
    /**
     * Represents an HTTP exception that produces a specific HTTP status code.
     *
     * Used to terminate request processing and return an HTTP error response.
     * Derive from this class to define custom HTTP exceptions or throw one of
     * Mach's built-in exception types.
     *
     * Ownership:
     * - Owns the exception message and associated HTTP status code.
     *
     * Thread safety:
     * - Immutable after construction and safe to access from multiple threads.
     */
    class HttpException : public std::runtime_error {

    public:
        /**
         * Initializes a new HTTP exception.
         *
         * @param status The HTTP status code to return.
         * @param message The error message describing the failure.
         */
        HttpException(http::StatusCode status, std::string message);

        /**
         * Returns the HTTP status code associated with this exception.
         *
         * @return The HTTP status code.
         */
        [[nodiscard]]
        http::StatusCode status() const noexcept;

    private:
        http::StatusCode m_status;
    };
}
