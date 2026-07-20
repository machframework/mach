#pragma once

#include <functional>
#include <stdexcept>

#include <mach/detail/middleware/InternalNext.hpp>
#include <mach/detail/dispatching/RequestExecution.hpp>

namespace mach
{
	class Next {

	public:
		Next(
			std::function<void()> next
		)
			: m_next(std::move(next))
		{ }

		Next(const Next&) = delete;
		Next& operator=(const Next&) = delete;

		void operator()() {
			if (m_invoked) {
				throw std::logic_error(
					"Mach error: next() may only be invoked once per middleware execution."
				);
			}

			m_invoked = true;
			m_next();
		}

	private:
		std::function<void()> m_next;
		bool m_invoked = false;
	};
}
