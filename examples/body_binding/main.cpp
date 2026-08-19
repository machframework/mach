#include <string>
#include <utility>

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
    auto builder = mach::AppBuilder();
    auto app = builder.build();

    app.mapPost("/users", [](CreateUserRequest request) {
        return mach::ok(
            UserProfile{
                .name = std::move(request.name),
                .age = request.age,
                .adult = request.age >= 18});
    });

    return app.run();
}
