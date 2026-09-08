#include <mach/mach.hpp>

int main() {
    mach::AppBuilder builder;

    builder.addCsrf([](mach::CsrfBuilder& csrf) {
        csrf.cookieName("__Host-csrf-token").headerName("X-CSRF-Token");
    });

    auto app = builder.build();

    app.mapGet("/profile", [] {
        return mach::ok("Profile");
    });

    app.mapPost("/profile", [] {
        return mach::ok("Profile updated");
    });

    return app.run();
}
