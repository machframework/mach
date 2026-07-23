#include <exception>
#include <iostream>
#include <thread>

#include <mach/mach.hpp>

int main()
{
    const auto hardwareThreads = std::thread::hardware_concurrency();

    auto builder = mach::AppBuilder("127.0.0.1", 3143, hardwareThreads);
    auto app = builder.build();

    app.mapGet("/", [] {
        return mach::ok("Hello, world!");
        });

    return app.run();
}
