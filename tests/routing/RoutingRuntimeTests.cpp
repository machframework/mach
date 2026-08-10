#include <iostream>
#include <thread>

#include <mach/mach.hpp>

#include "Testing.hpp"


int main() {
    auto builder = mach::AppBuilder(
        mach::AppOptions{
            .host = "127.0.0.1",
            .port = 3143,
            .threadCount = std::thread::hardware_concurrency()});

    auto app = builder.build();

    // -------------------------
    // Root route
    // -------------------------
    {
        app.mapGet("/", [](mach::Context& context) {
            context.response.body("Root reached");
        });
    }

    // -------------------------
    // Static route
    // -------------------------
    {
        app.mapGet("/runtime/static", [](mach::Context& context) {
            context.response.body("static route reached");
        });
    }

    // -------------------------
    // Static route beats parameter route
    // -------------------------
    {
        app.mapGet("/runtime/precedence/{id}", [](mach::Context& context) {
            context.response.body("parameter route reached");
        });

        app.mapGet("/runtime/precedence/me", [](mach::Context& context) {
            context.response.body("static route reached");
        });
    }

    // -------------------------
    // Parameter extraction
    // -------------------------
    { // problematic for next one
        app.mapGet("/runtime/users/{id}", [](mach::Context& context) {
            context.response.body("user id: " + std::string(context.request.routeParam("id")));
        });
    }

    // -------------------------
    // Multiple parameter extraction
    // -------------------------
    {
        app.mapGet("/runtime/users/{userId}/posts/{postId}", [](mach::Context& context) {
            context.response.body(
                "user id: " + std::string(context.request.routeParam("userId")) +
                ", post id: " + std::string(context.request.routeParam("postId")));
        });
    }

    // -------------------------
    // Int constraint
    // -------------------------
    {
        app.mapGet("/runtime/orders/{id:int}", [](mach::Context& context) {
            context.response.body("order id: " + std::string(context.request.routeParam("id")));
        });
    }

    // -------------------------
    // MethodNotAllowed route
    // -------------------------
    {
        app.mapGet("/runtime/get-only", [](mach::Context& context) {
            context.response.body("GET route reached");
        });
    }

    // -------------------------
    // POST route
    // -------------------------
    {
        app.mapPost("/runtime/post-route", [](mach::Context& context) {
            context.response.body("POST route reached");
        });
    }

    // -------------------------
    // Static branch fallback to parameter route
    // -------------------------
    {
        app.mapGet("/runtime/me/profile", [](mach::Context& context) {
            context.response.body("static route reached");
        });

        app.mapGet("/runtime/{username}/posts", [](mach::Context& context) {
            context.response.body(
                "parameter route reached: " + std::string(context.request.routeParam("username")));
        });
    }

    // -------------------------
    // Static branch fallback to parameter route with double backtracking
    // -------------------------
    {
        app.mapGet("/runtime/me/profile/details", [](mach::Context& context) {
            context.response.body("first static route reached");
        });

        app.mapGet("/runtime/{username}/posts/archive/details", [](mach::Context& context) {
            context.response.body("second static route reached");
        });

        app.mapGet("/runtime/{username}/posts/{postId}/comments", [](mach::Context& context) {
            context.response.body(
                "username: " + std::string(context.request.routeParam("username")) +
                ", post id: " + std::string(context.request.routeParam("postId")));
        });
    }

    std::cout << testing::GREEN
              << "[INFO] Routing runtime test server running on http://127.0.0.1:3143"
              << testing::RESET << std::endl;

    std::cout << "Waiting for manual checks:\n"
              << "\n"

              << "GET  /runtime/static\n"
              << "=> 200, 'static route reached'\n"
              << "\n"

              << "GET  /runtime/precedence/me\n"
              << "=> 200, 'static route reached'\n"
              << "\n"

              << "GET  /runtime/precedence/asaf\n"
              << "=> 200, 'parameter route reached'\n"
              << "\n"

              << "GET  /runtime/users/123\n"
              << "=> 200, 'user id: 123'\n"
              << "\n"

              << "GET  /runtime/users/asaf/posts/42\n"
              << "=> 200, 'user id: asaf, post id: 42'\n"
              << "\n"

              << "GET  /runtime/orders/123\n"
              << "=> 200, 'order id: 123'\n"
              << "\n"

              << "GET  /runtime/orders/abc\n"
              << "=> 404\n"
              << "\n"

              << "GET  /runtime/orders/-123\n"
              << "=> 200, 'order id: -123'\n"
              << "\n"

              << "GET  /runtime/orders/+123\n"
              << "=> 404\n"
              << "\n"

              << "POST /runtime/get-only\n"
              << "=> 405\n"
              << "\n"

              << "POST /runtime/post-route\n"
              << "=> 200, 'POST route reached'\n"
              << "\n"

              << "GET  /runtime/does-not-exist\n"
              << "=> 404\n"

              << "GET /runtime/me/posts\n"
              << "=> 200, 'parameter route reached: me'\n"
              << "\n"

              << "GET /runtime/me/posts/archive/comments\n"
              << "=> 200, 'username: me, post id: archive'\n"
              << "\n"

              << std::endl;

    return app.run();
}