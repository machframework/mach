#include <mach/exceptions/BadRequestException.hpp>

#include <utility>

#include <mach/http/StatusCode.hpp>

namespace mach
{
    BadRequestException::BadRequestException(std::string message)
        : HttpException(http::StatusCode::BadRequest, std::move(message)) {}
}
