#include <exception>
#include <iostream>
#include <thread>

#include <mach/mach.hpp>

int main() {
    auto builder = mach::AppBuilder();

    builder.configureCors([](mach::CorsBuilder& cors) {
        cors.allowOrigin("https://example.com");
        cors.allowMethod(mach::http::Method::Post);
        cors.maxAge(std::chrono::seconds(3600));
    });

    auto app = builder.build();

    app.mapGet("/", [] {
        return mach::ok("Hello, world!");
    });

    return app.run();
}
