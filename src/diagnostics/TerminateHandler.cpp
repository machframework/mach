#include <mach/diagnostics/TerminateHandler.hpp>

#include <cstdio>
#include <cstdlib>
#include <exception>

namespace
{
    void logFatalError(const char* message) noexcept {
        std::fputs(message, stderr);
        std::fputc('\n', stderr);
        std::fflush(stderr);
    }

    [[noreturn]]
    void machTerminateHandler() noexcept {
        const auto exception = std::current_exception();

        if (exception == nullptr) {
            logFatalError("std::terminate was called without an active exception");

            std::abort();
        }

        try {
            std::rethrow_exception(exception);
        } catch (const std::exception& error) {
            logFatalError(error.what());
        } catch (...) {
            logFatalError("Unhandled exception not derived from std::exception");
        }

        std::abort();
    }
}

namespace mach
{
    void installTerminateHandler() noexcept {
        std::set_terminate(machTerminateHandler);
    }
}