#pragma once

#include <functional>

namespace mach::detail::dispatching
{
	class RequestExecution;
}

namespace mach::detail::middleware
{
	using Next = std::function<void(detail::dispatching::RequestExecution&)>;
}
