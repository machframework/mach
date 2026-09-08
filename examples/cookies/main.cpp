#include <chrono>
#include <string>

#include <mach/mach.hpp>

int main() {
    mach::AppBuilder builder;
    auto app = builder.build();

    app.mapGet("/login", [](mach::Context& context) {
        context.response.addCookie(
            mach::http::Cookie{
                .name = "session",
                .value = "example-session-token",
                .maxAge = std::chrono::hours(1),
                .sameSite = mach::http::SameSite::Lax,
                .secure = true,
                .httpOnly = true});

        return mach::ok("Session cookie set");
    });

    app.mapGet("/session", [](mach::Context& context) {
        const auto session = context.request.cookie("session");

        if (!session) {
            return mach::unauthorized<std::string>();
        }

        return mach::ok("Session cookie received");
    });

    return app.run();
}
