#include <mach/exceptions/ForbiddenException.hpp>

#include <utility>

#include <mach/http/StatusCode.hpp>

namespace mach
{
    ForbiddenException::ForbiddenException(std::string message)
        : HttpException(http::StatusCode::Forbidden, std::move(message)) {}
}
