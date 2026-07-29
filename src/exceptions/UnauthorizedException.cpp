#include <mach/exceptions/UnauthorizedException.hpp>

#include <utility>

#include <mach/http/StatusCode.hpp>

namespace mach
{
    UnauthorizedException::UnauthorizedException(std::string message)
        : HttpException(http::StatusCode::Unauthorized, std::move(message)) {}
}
