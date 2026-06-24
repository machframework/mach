#pragma once

#include <functional>

#include <mach/detail/dispatching/RequestExecution.hpp>

namespace mach
{
	using Next = std::function<void()>;
}
