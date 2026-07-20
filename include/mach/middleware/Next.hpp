#pragma once

#include <functional>
#include <stdexcept>

#include <mach/detail/middleware/InternalNext.hpp>
#include <mach/detail/dispatching/RequestExecution.hpp>

namespace mach
{
	class Next {

	public:
		Next(std::function<void()> next);

		Next(const Next&) = delete;
		Next& operator=(const Next&) = delete;

		void operator()();

	private:
		std::function<void()> m_next;
		bool m_invoked = false;
	};
}
