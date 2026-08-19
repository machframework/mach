#include <string>

#include <mach/mach.hpp>

int main() {
    auto builder = mach::AppBuilder();
    auto app = builder.build();

    app.mapGet("/users/{id:int}", [](mach::Context& context) {
        const int id = context.request.routeParam<int>("id");

        if (id != 42) {
            return mach::notFound<std::string>();
        }

        return mach::ok("User found");
    });

    app.mapGet("/admin", [](mach::Context& context) {
        const auto authorization = context.request.header("Authorization");

        if (!authorization) {
            return mach::unauthorized<std::string>();
        }

        return mach::ok("Welcome to the admin area");
    });

    return app.run();
}
