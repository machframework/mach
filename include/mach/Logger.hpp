#pragma once

#include <memory>
#include <string_view>
#include <utility>

#include <fmt/format.h>

namespace mach::detail::di
{
    class Container;
}

namespace mach
{
    class AppBuilder;

    class Logger {

    public:
        ~Logger();

        Logger(Logger&&) noexcept;

        Logger(const Logger&) = delete;
        Logger& operator=(const Logger&) = delete;

        template <typename... Args>
        void debug(fmt::format_string<Args...> format, Args&&... args) const {
            if (!shouldLog(LogLevel::Debug)) {
                return;
            }

            log(LogLevel::Debug, fmt::format(format, std::forward<Args>(args)...));
        }

        template <typename... Args>
        void info(fmt::format_string<Args...> format, Args&&... args) const {
            if (!shouldLog(LogLevel::Info)) {
                return;
            }

            log(LogLevel::Info, fmt::format(format, std::forward<Args>(args)...));
        }

        template <typename... Args>
        void warning(fmt::format_string<Args...> format, Args&&... args) const {
            if (!shouldLog(LogLevel::Warning)) {
                return;
            }

            log(LogLevel::Warning, fmt::format(format, std::forward<Args>(args)...));
        }

        template <typename... Args>
        void error(fmt::format_string<Args...> format, Args&&... args) const {
            if (!shouldLog(LogLevel::Error)) {
                return;
            }

            log(LogLevel::Error, fmt::format(format, std::forward<Args>(args)...));
        }

    private:
        Logger();

        Logger& operator=(Logger&&) noexcept;

        enum class LogLevel {
            Debug,
            Info,
            Warning,
            Error
        };

        [[nodiscard]]
        bool shouldLog(LogLevel level) const noexcept;

        void log(LogLevel level, std::string_view message) const;

        class Impl;
        std::unique_ptr<Impl> m_impl;

        friend class AppBuilder;
    };
}
