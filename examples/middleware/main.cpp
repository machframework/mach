#include <iostream>
#include <thread>

#include <mach/mach.hpp>
#include <mach/middleware/Next.hpp>

class LoggingMiddleware {

public:
    LoggingMiddleware(mach::Logger& logger) : m_logger(logger) {}

    void invoke(mach::Context& context, mach::Next& next) {
        m_logger.info("Before handler");

        context.response.setHeader("X-Middleware", "Logging");
        next();

        m_logger.info("After handler");
    }

private:
    mach::Logger& m_logger;
};

int main() {
    const auto hardwareThreads = std::thread::hardware_concurrency();

    auto builder = mach::AppBuilder(
        mach::ServerOptions{
            .host = "127.0.0.1",
            .port = 3143,
            .threadCount = std::thread::hardware_concurrency()});

    builder.use<LoggingMiddleware, mach::Logger>();

    auto app = builder.build();

    app.mapGet("/test", [] {
        std::cout << "Inside handler\n";
        return mach::ok("Reached");
    });

    return app.run();
}
