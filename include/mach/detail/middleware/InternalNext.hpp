#pragma once

#include <functional>

namespace mach::detail::dispatching
{
    struct RequestExecution;
}

namespace mach::detail::middleware
{
    using InternalNext = std::function<void(dispatching::RequestExecution&)>;
}
