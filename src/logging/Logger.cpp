#include <mach/Logger.hpp>

#include <memory>
#include <utility>

#include <spdlog/logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace mach
{
    class Logger::Impl {

    public:
        explicit Impl(std::shared_ptr<spdlog::logger> logger) : logger(std::move(logger)) {}

        std::shared_ptr<spdlog::logger> logger;
    };

    Logger::Logger() {
        auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

        consoleSink->set_pattern("[%H:%M:%S] [%^%l%$] %v");

        auto logger = std::make_shared<spdlog::logger>("mach", std::move(consoleSink));

        logger->set_level(spdlog::level::info);
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
