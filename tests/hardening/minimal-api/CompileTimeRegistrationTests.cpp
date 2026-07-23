#include <mach/App.hpp>
#include <mach/AppBuilder.hpp>

#include "Testing.hpp"

static void doNothing(mach::Context& context) {
    context.response.setHeader("X-Arrived", "1");
}

int main() {
    mach::AppBuilder builder(std::move(testing::serverOptions));
    auto app = builder.build();

    int count = 0;

    app.mapGet("/noexcept", [count]() mutable noexcept {
        count++;
    });

    app.mapGet("/free-function", doNothing);

    app.mapGet("/pointer", &doNothing);

    return app.run();
}