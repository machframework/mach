#pragma once

#include <functional>

#include <mach/Context.hpp>

namespace mach
{
	using Next = std::function<void(mach::Context&)>;
}
