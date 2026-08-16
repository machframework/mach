#include <string>
#include <thread>
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

    builder.configureApp([](mach::AppOptions& options) {
        options.host = "127.0.0.1";
        options.port = 3143;
        options.threadCount = std::thread::hardware_concurrency();
    });

    auto app = builder.build();

    app.mapPost("/users", [](CreateUserRequest request) {
        return mach::ok(
            UserProfile{
                .name = std::move(request.name),
                .age = request.age,
                .adult = request.age >= 18});
    });

    app.mapPost("/users/debug", [](mach::Context& context, CreateUserRequest request) {
        context.response.setHeader("X-User-Age", std::to_string(request.age));

        return mach::ok(
            UserProfile{
                .name = std::move(request.name),
                .age = request.age,
                .adult = request.age >= 18});
    });

    return app.run();
}
