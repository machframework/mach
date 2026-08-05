#include <exception>
#include <iostream>
#include <thread>

#include <mach/mach.hpp>

int main() {
    auto builder = mach::AppBuilder();

    builder.configureCors([](mach::CorsBuilder& cors) {
        cors.allowAnyOrigin();
        cors.allowAnyMethod();
        cors.allowAnyHeader();
    });

    auto app = builder.build();

    app.mapGet("/", [] {
        return mach::ok("Hello, world!");
    });

    return app.run();
}
