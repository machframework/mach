#include <mach/mach.hpp>

int main() {
    mach::AppBuilder builder;
    auto app = builder.build();

    app.mapGet("/", [] {
        return mach::ok("Hello from Mach!");
    });

    return app.run();
}
