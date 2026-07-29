#include <mach/exceptions/HttpException.hpp>

namespace mach
{
    HttpException::HttpException(http::StatusCode statusCode, std::string message)
        : std::runtime_error(std::move(message)), m_status(statusCode) {}

    http::StatusCode HttpException::status() const noexcept {
        return m_status;
    }
}
