#include <mach/Logger.hpp>

#include <spdlog/logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/stdout_sinks.h>

namespace
{
    spdlog::level::level_enum toSpdlogLevel(mach::LogLevel level) {
        switch (level) {
        case mach::LogLevel::Debug:
            return spdlog::level::debug;

        case mach::LogLevel::Info:
            return spdlog::level::info;

        case mach::LogLevel::Warning:
            return spdlog::level::warn;

        case mach::LogLevel::Error:
            return spdlog::level::err;
        }

        return spdlog::level::off;
    }
}

namespace mach
{
    class Logger::Impl {

    public:
        explicit Impl(std::shared_ptr<spdlog::logger> logger) : logger(std::move(logger)) {}

        std::shared_ptr<spdlog::logger> logger;
    };

    Logger::Logger(mach::LoggerOptions&& options) {
        std::shared_ptr<spdlog::sinks::sink> consoleSink;

        if (options.enableColors) {
            consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        } else {
            consoleSink = std::make_shared<spdlog::sinks::stdout_sink_mt>();
        }

        consoleSink->set_pattern(options.pattern);

        auto logger = std::make_shared<spdlog::logger>("mach", std::move(consoleSink));

        logger->set_level(toSpdlogLevel(options.level));
        logger->flush_on(spdlog::level::err);

        m_impl = std::make_unique<Impl>(std::move(logger));
    }

    Logger::~Logger() = default;

    Logger::Logger(Logger&&) noexcept = default;

    Logger& Logger::operator=(Logger&&) noexcept = default;

    bool Logger::shouldLog(const LogLevel level) const noexcept {
        switch (level) {
        case LogLevel::Debug:
            return m_impl->logger->should_log(spdlog::level::debug);

        case LogLevel::Info:
            return m_impl->logger->should_log(spdlog::level::info);

        case LogLevel::Warning:
            return m_impl->logger->should_log(spdlog::level::warn);

        case LogLevel::Error:
            return m_impl->logger->should_log(spdlog::level::err);
        }

        return false;
    }

    void Logger::log(const LogLevel level, const std::string_view message) const {
        switch (level) {
        case LogLevel::Debug:
            m_impl->logger->debug("{}", message);
            break;

        case LogLevel::Info:
            m_impl->logger->info("{}", message);
            break;

        case LogLevel::Warning:
            m_impl->logger->warn("{}", message);
            break;

        case LogLevel::Error:
            m_impl->logger->error("{}", message);
            break;
        }
    }
}
