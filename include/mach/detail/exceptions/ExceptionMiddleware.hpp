#pragma once

#include <string>
#include <utility>

#include <mach/Context.hpp>
#include <mach/middleware/Next.hpp>

namespace mach::detail::exceptions
{
	class ExceptionMiddleware {

	public:
		explicit ExceptionMiddleware() = default;

		void invoke(mach::Context& context, mach::Next& next);
	};
}
