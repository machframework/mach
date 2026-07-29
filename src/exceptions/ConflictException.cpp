#include <mach/exceptions/ConflictException.hpp>

#include <utility>

#include <mach/http/StatusCode.hpp>

namespace mach
{
    ConflictException::ConflictException(std::string message)
        : HttpException(http::StatusCode::Conflict, std::move(message)) {}
}
