#include <iostream>
#include <thread>

#include <mach/mach.hpp>
#include <mach/middleware/Next.hpp>

class LoggingMiddleware {

public:
    void invoke(mach::Context& context, mach::Next& next) {
        std::cout << "Before handler\n";

        context.response.setHeader("X-Middleware", "Logging");
        next();

        std::cout << "After handler\n";
    }
};

int main()
{
    const auto hardwareThreads = std::thread::hardware_concurrency();

    auto builder = mach::AppBuilder("127.0.0.1", 3143, hardwareThreads);

    builder.use<LoggingMiddleware>();

    auto app = builder.build();

    app.mapGet("/test", [] {
        std::cout << "Inside handler\n";
        return mach::ok("Reached");
        });

    return app.run();
}
