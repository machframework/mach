#include <mach/Context.hpp>

#include <utility>

namespace mach
{
    Context::Context(Request&& request, Response&& response)
        : request(std::move(request)), response(std::move(response)) {}
}
