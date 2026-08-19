#include <string>

#include <mach/mach.hpp>

int main() {
    auto builder = mach::AppBuilder();
    auto app = builder.build();

    app.mapGet("/", [] {
        return mach::ok("Home");
    });

    app.mapGet("/users/{id:int}", [](mach::Context& context) {
        const int id = context.request.routeParam<int>("id");
        return mach::ok("User " + std::to_string(id));
    });

    app.mapGet("/articles/{title}", [](mach::Context& context) {
        const auto title = context.request.routeParam("title");
        return mach::ok("Article: " + std::string(title));
    });

    app.mapPost("/users", [] {
        return mach::created();
    });

    app.mapDelete("/users/{id:int}", [](mach::Context& context) {
        const int id = context.request.routeParam<int>("id");
        return mach::noContent();
    });

    return app.run();
}
