#include <mach/exceptions/NotFoundException.hpp>

#include <utility>

#include <mach/http/StatusCode.hpp>

namespace mach
{
    NotFoundException::NotFoundException(std::string message)
        : HttpException(http::StatusCode::NotFound, std::move(message)) {}
}
