#include "mach/logging/Logging.hpp"

#include <iostream>

namespace mach::detail::logging
{
    using std::cerr;
    using std::cout;
    using std::endl;

    void Logger::debug(std::string_view message) {
        log(LogLevel::Debug, message);
    }

    void Logger::info(std::string_view message) {
        log(LogLevel::Info, message);
    }

    void Logger::warning(std::string_view message) {
        log(LogLevel::Warning, message);
    }

    void Logger::error(std::string_view message) {
        log(LogLevel::Error, message);
    }

    void Logger::log(LogLevel level, std::string_view message) {
        switch (level) {
        case LogLevel::Debug:
            cout << "[DEBUG] " << message << endl;
            break;
        case LogLevel::Info:
            cout << "[INFO] " << message << endl;
            break;
        case LogLevel::Warning:
            cerr << "[WARNING] " << message << endl;
            break;
        case LogLevel::Error:
            cerr << "[ERROR] " << message << endl;
            break;
        }
    }
}
