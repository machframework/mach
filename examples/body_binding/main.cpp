#include <iostream>
#include <string>

#include <mach/AppBuilder.hpp>
#include <mach/Json.hpp>
#include <mach/results/Results.hpp>

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

    app.get("/users/create", [](CreateUserRequest request) {
        const bool adult = request.age >= 18;

        std::cout << "Creating user: "
            << request.name << ", age " << request.age << "\n";

        return mach::results::Results::ok(UserProfile{
            .name = request.name,
            .age = request.age,
            .adult = adult
            });
        });

    app.get("/debug/context", [](mach::Context& context) {
        context.response.setHeader("X-Mach-Example", "body-binding");

        return mach::results::Results::ok("Context reached");
        });

    app.get("/debug/context/request", [](mach::Context& context, CreateUserRequest request) {
        context.response.setHeader("X-Age", std::to_string(request.age));
        context.response.setHeader("X-Name", request.name);

        return mach::results::Results::ok("Context + DTO reached");
        });

    app.get("/debug/request/context", [](CreateUserRequest request, mach::Context& context) {
        context.response.setHeader("X-Age", std::to_string(request.age));
        context.response.setHeader("X-Name", request.name);

        return mach::results::Results::ok("DTO + Context reached");
        });

    app.run();

    return 0;
}