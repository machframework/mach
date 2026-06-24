#pragma once

#include <functional>

namespace mach
{
	class Context;

	namespace detail
	{
		using MinimalApiHandler = std::function<void(Context&)>;
	}
}
