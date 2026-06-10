#pragma once

#include <functional>

namespace mach
{
	class Context;
	
	namespace detail
	{
		using Handler = std::function<void(Context&)>;
	}
}
