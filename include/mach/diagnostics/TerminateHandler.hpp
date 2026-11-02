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
     * @deprecated Since v1.1.0. The termination handler is now installed
     *             automatically by AppBuilder::build() unless explicitly disabled.
     *
     * @thread_safety This function is not thread-safe.
     */
    [[deprecated(
        "Since v1.1.0: the terminate handler is installed automatically by AppBuilder::build().")]]
    void installTerminateHandler() noexcept;
}
