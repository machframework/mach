#include <mach/exceptions/HttpException.hpp>

namespace mach
{
    HttpException::HttpException(http::StatusCode status, std::string message)
        : std::runtime_error(std::move(message)), m_status(status) {}

    http::StatusCode HttpException::status() const noexcept {
        return m_status;
    }
}
