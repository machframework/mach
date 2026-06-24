#pragma once

#include <functional>

#include <mach/detail/dispatching/RequestExecution.hpp>

namespace mach::detail::middleware
{
	using Next = std::function<void(detail::dispatching::RequestExecution&)>;
}
