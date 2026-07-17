#include <iostream>
#include <string>

#include <mach/AppBuilder.hpp>
#include <mach/Json.hpp>
#include <mach/results/Results.hpp>

#include <mutex>

struct CreateUserRequest {
    std::string name;
    int age;
};

MACH_DEFINE_JSON(CreateUserRequest, name, age)

struct UserProfile {
    std::string name;
    int age;
    bool adult;
};

MACH_DEFINE_JSON(UserProfile, name, age, adult)

int main() {
    auto builder = mach::AppBuilder("127.0.0.1", 3143, 16);
    auto app = builder.build();

    app.mapGet("/", [](std::mutex mtx) {

        });

    app.mapGet("/users/create", [](CreateUserRequest request) {
        const bool adult = request.age >= 18;

        std::cout << "Creating user: "
            << request.name << ", age " << request.age << "\n";

        return mach::results::ok(UserProfile{
            .name = request.name,
            .age = request.age,
            .adult = adult
            });
        });

    app.mapGet("/debug/context", [](mach::Context& context) {
        context.response.setHeader("X-Mach-Example", "body-binding");

        return mach::results::ok("Context reached");
        });

    app.mapGet("/debug/context/request", [](mach::Context& context, CreateUserRequest request) {
        context.response.setHeader("X-Age", std::to_string(request.age));
        context.response.setHeader("X-Name", request.name);

        return mach::results::ok("Context + DTO reached");
        });

    app.mapGet("/debug/request/context", [](CreateUserRequest request, mach::Context& context) {
        context.response.setHeader("X-Age", std::to_string(request.age));
        context.response.setHeader("X-Name", request.name);

        return mach::results::ok("DTO + Context reached");
        });

    return app.run();;
}
