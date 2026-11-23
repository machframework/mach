#include <string>

#include <mach/mach.hpp>

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
    mach::AppBuilder builder;
    auto app = builder.build();

    app.mapPost("/users", [](mach::Body<CreateUserRequest> body) {
        return mach::ok(
            UserProfile{.name = body->name, .age = body->age, .adult = body->age >= 18});
    });

    return app.run();
}
