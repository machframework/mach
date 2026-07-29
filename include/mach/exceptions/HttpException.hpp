#pragma once

#include <stdexcept>
#include <string>

#include <mach/http/StatusCode.hpp>

namespace mach
{
    class HttpException : public std::runtime_error {

    public:
        HttpException(http::StatusCode status, std::string message);

        [[nodiscard]]
        http::StatusCode status() const noexcept;

    private:
        http::StatusCode m_status;
    };
}
