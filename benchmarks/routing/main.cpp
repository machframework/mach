#include <thread>

#include <mach/App.hpp>
#include <mach/AppBuilder.hpp>
#include <mach/Context.hpp>

int main() {
    int threads = std::thread::hardware_concurrency();

    mach::AppBuilder builder;
    auto app = builder.build();

    // root
    app.mapGet("/", [](mach::Context& context) {
        context.response.body("Ok root");
    });

    // static route lookup
    app.mapGet("/runtime/static", [](mach::Context& context) {
        context.response.body("Ok static route");
    });

    // single parameter extraction
    app.mapGet("/runtime/users/{userId:int}", [](mach::Context& context) {
        context.response.body("Ok single param");
    });

    // multiple parameters
    app.mapGet("/runtime/users/{userName}/posts/{postId:int}", [](mach::Context& context) {
        context.response.body("Ok multiple params");
    });

    // static-vs-param precedence path
    app.mapGet("/runtime/precedence/me", [](mach::Context& context) {
        context.response.body("Ok precedence");
    });

    app.mapGet("/runtime/precedence/{userName}", [](mach::Context& context) {
        context.response.body("Reached when name isn't 'me'");
    });

    app.run();
}
