#pragma once

#include <memory>
#include <string_view>
#include <utility>

#include <fmt/format.h>

#include <mach/LogLevel.hpp>
#include <mach/LoggerOptions.hpp>

namespace mach
{
    class AppBuilder;

    /**
     * Provides thread-safe structured logging for a Mach application.
     *
     * Used to write structured log messages from application code and framework
     * components. Supports configurable log levels, formatting patterns, and
     * colour output.
     *
     * Ownership:
     * - Created and owned by the Mach application.
     * - Available through dependency injection after the application is built.
     * - Lifetime is tied to the application's lifetime.
     *
     * Thread safety:
     * - Thread-safe. All member functions may be called concurrently from
     *   multiple threads.
     */
    class Logger {

    public:
        ~Logger();

        Logger(Logger&&) noexcept;

        Logger(const Logger&) = delete;
        Logger& operator=(const Logger&) = delete;

        /**
         * Logs a debug message if debug logging is enabled.
         *
         * The message is formatted using the {fmt} formatting library. Formatting
         * arguments are validated at compile time.
         *
         * @tparam Args The types of the formatting arguments.
         * @param format The compile-time checked format string.
         * @param args The values used to format the message.
         *
         * @thread_safety This function is thread-safe.
         */
        template <typename... Args>
        void debug(fmt::format_string<Args...> format, Args&&... args) const;

        /**
         * Logs an info message if info logging is enabled.
         *
         * The message is formatted using the {fmt} formatting library. Formatting
         * arguments are validated at compile time.
         *
         * @tparam Args The types of the formatting arguments.
         * @param format The compile-time checked format string.
         * @param args The values used to format the message.
         *
         * @thread_safety This function is thread-safe.
         */
        template <typename... Args>
        void info(fmt::format_string<Args...> format, Args&&... args) const;

        /**
         * Logs a warning message if warning logging is enabled.
         *
         * The message is formatted using the {fmt} formatting library. Formatting
         * arguments are validated at compile time.
         *
         * @tparam Args The types of the formatting arguments.
         * @param format The compile-time checked format string.
         * @param args The values used to format the message.
         *
         * @thread_safety This function is thread-safe.
         */
        template <typename... Args>
        void warning(fmt::format_string<Args...> format, Args&&... args) const;

        /**
         * Logs an error message if error logging is enabled.
         *
         * The message is formatted using the {fmt} formatting library. Formatting
         * arguments are validated at compile time.
         *
         * @tparam Args The types of the formatting arguments.
         * @param format The compile-time checked format string.
         * @param args The values used to format the message.
         *
         * @thread_safety This function is thread-safe.
         */
        template <typename... Args>
        void error(fmt::format_string<Args...> format, Args&&... args) const;

    private:
        explicit Logger(mach::LoggerOptions&& options);

        Logger& operator=(Logger&&) noexcept;

        [[nodiscard]]
        bool shouldLog(LogLevel level) const noexcept;

        void log(LogLevel level, std::string_view message) const;

        class Impl;
        std::unique_ptr<Impl> m_impl;

        friend class AppBuilder;
    };

    template <typename... Args>
    void Logger::debug(fmt::format_string<Args...> format, Args&&... args) const {
        if (!shouldLog(LogLevel::Debug)) {
            return;
        }

        log(LogLevel::Debug, fmt::format(format, std::forward<Args>(args)...));
    }

    template <typename... Args>
    void Logger::info(fmt::format_string<Args...> format, Args&&... args) const {
        if (!shouldLog(LogLevel::Info)) {
            return;
        }

        log(LogLevel::Info, fmt::format(format, std::forward<Args>(args)...));
    }

    template <typename... Args>
    void Logger::warning(fmt::format_string<Args...> format, Args&&... args) const {
        if (!shouldLog(LogLevel::Warning)) {
            return;
        }

        log(LogLevel::Warning, fmt::format(format, std::forward<Args>(args)...));
    }

    template <typename... Args>
    void Logger::error(fmt::format_string<Args...> format, Args&&... args) const {
        if (!shouldLog(LogLevel::Error)) {
            return;
        }

        log(LogLevel::Error, fmt::format(format, std::forward<Args>(args)...));
    }
}
