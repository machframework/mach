#pragma once

#include <functional>
#include <stdexcept>

#include <mach/detail/middleware/InternalNext.hpp>
#include <mach/detail/dispatching/RequestExecution.hpp>

namespace mach
{
	/**
	 * Represents the next step in the middleware pipeline.
	 *
	 * Calling next() transfers execution to the next middleware or endpoint
	 * in the request pipeline. A Next instance may only be invoked once.
	 */
	class Next {

	public:

		/**
		 * Constructs a Next object wrapping the next pipeline step.
		 *
		 * @param next The function to invoke when continuing pipeline
		 *             execution.
		 */
		explicit Next(std::function<void()> next);

		Next(const Next&) = delete;
		Next& operator=(const Next&) = delete;

		/**
		 * Continues execution of the middleware pipeline.
		 *
		 * This function may only be called once. Calling it more than once
		 * results in a std::logic_error being thrown.
		 *
		 * @throws std::logic_error If next() has already been invoked.
		 */
		void operator()();

	private:
		std::function<void()> m_next;
		bool m_invoked = false;
	};
}
