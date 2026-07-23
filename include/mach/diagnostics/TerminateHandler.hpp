#pragma once

namespace mach
{
	/**
	 * Installs Mach's global termination handler.
	 *
	 * Replaces the current std::terminate handler with Mach's implementation,
	 * allowing unhandled fatal errors to be logged before the application
	 * terminates.
	 *
	 * This function affects the entire process and should typically be called
	 * once during application startup.
	 *
	 * @thread_safety This function is not thread-safe.
	 */
	void installTerminateHandler() noexcept;
}
