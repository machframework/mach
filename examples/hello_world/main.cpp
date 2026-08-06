#include <exception>
#include <iostream>
#include <thread>

#include <mach/mach.hpp>

int main() {
    auto builder = mach::AppBuilder();
    auto app = builder.build();

    app.mapGet("/", [] {
        return mach::ok("Hello, world!");
    });

    return app.run();
}
