#include <mach/mach.hpp>
#include <mach/middleware/Next.hpp>

class LoggingMiddleware {

public:
    explicit LoggingMiddleware(mach::Logger& logger) : m_logger(logger) {}

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
    auto builder = mach::AppBuilder();

    builder.use<LoggingMiddleware, mach::Logger>();

    auto app = builder.build();

    app.mapGet("/hello", [] {
        return mach::ok("Hello through middleware!");
    });

    return app.run();
}
