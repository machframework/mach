#include <string>

#include <mach/mach.hpp>

int main() {
    mach::AppBuilder builder;
    auto app = builder.build();

    app.mapGet("/", [] {
        return mach::ok("Home");
    });

    app.mapGet("/users/{id:int}", [](mach::Param<int, "id"> id) {
        return mach::ok("User " + std::to_string(*id));
    });

    app.mapGet("/articles/{title}", [](mach::Param<std::string, "title"> title) {
        return mach::ok("Article: " + *title);
    });

    app.mapPost("/users", [] {
        return mach::created();
    });

    app.mapDelete("/users/{id:int}", [](mach::Param<int, "id"> id) {
        return mach::noContent();
    });

    return app.run();
}
