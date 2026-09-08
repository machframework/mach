#include <chrono>

#include <mach/mach.hpp>

int main() {
    mach::AppBuilder builder;

    builder.addCors([](mach::CorsBuilder& cors) {
        cors.allowOrigin("https://example.com")
            .allowMethods({mach::http::Method::Get, mach::http::Method::Post})
            .allowHeaders({"Content-Type", "Authorization"})
            .allowCredentials()
            .maxAge(std::chrono::hours(1));
    });

    auto app = builder.build();

    app.mapGet("/api/message", [] {
        return mach::ok("Hello from Mach!");
    });

    return app.run();
}