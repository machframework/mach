#include <string>
#include <string_view>
#include <utility>

#include <mach/controllers.hpp>
#include <mach/mach.hpp>

// ---------------------------------------------------------
// Models
// ---------------------------------------------------------

struct UserProfile {
    std::string name;
    int age;
    bool adult;
};

MACH_DEFINE_JSON(UserProfile, name, age, adult)

struct User {
    int id;
    std::string name;
    int age;
    bool adult;
};

MACH_DEFINE_JSON(User, id, name, age, adult)

// ---------------------------------------------------------
// Service
// ---------------------------------------------------------

class UserService {
public:
    User update(int id, UserProfile profile) const {
        return User{
            .id = id,
            .name = std::move(profile.name),
            .age = profile.age,
            .adult = profile.adult};
    }
};

// ---------------------------------------------------------
// Controller
// ---------------------------------------------------------

class UsersController : public mach::ControllerBase {
public:
    inline static constexpr std::string_view route = "/controller/users";

    explicit UsersController(UserService& users) : m_users(users) {}

    mach::Reply<User> updateDirect(UserProfile profile) {
        const int id = request().routeParam<int>("id");

        return ok(
            User{
                .id = id,
                .name = std::move(profile.name),
                .age = profile.age,
                .adult = profile.adult});
    }

    mach::Reply<User> updateWithService(UserProfile profile) {
        const int id = request().routeParam<int>("id");
        return ok(m_users.update(id, std::move(profile)));
    }

    static void configure(mach::ControllerBuilder<UsersController>& routes) {
        routes.mapPost("/{id}/direct", &UsersController::updateDirect);

        routes.mapPost("/{id}/service", &UsersController::updateWithService);
    }

private:
    UserService& m_users;
};

// ---------------------------------------------------------
// Application
// ---------------------------------------------------------

int main() {
    mach::AppBuilder builder;

    builder.addSingleton<UserService>();
    builder.addController<UsersController, UserService>();

    auto app = builder.build();

    // 1. Basic HTTP request/response
    app.mapGet("/plaintext", [] {
        return mach::ok("Hello, World!");
    });

    // 2. JSON serialization
    app.mapGet("/json", [] {
        return mach::ok(UserProfile{.name = "Alice", .age = 28, .adult = true});
    });

    // 3. Parameterized routing + typed route extraction
    app.mapGet("/users/{id}", [](mach::Context& context) {
        const int id = context.request.routeParam<int>("id");

        return mach::ok(User{.id = id, .name = "Alice", .age = 28, .adult = true});
    });

    // 4. JSON model binding + serialization
    app.mapPost("/users", [](UserProfile profile) {
        return mach::ok(profile);
    });

    // 5. Representative minimal API endpoint
    app.mapPost("/minimal/users/{id}", [](mach::Context& context, UserProfile profile) {
        const int id = context.request.routeParam<int>("id");

        return mach::ok(
            User{
                .id = id,
                .name = std::move(profile.name),
                .age = profile.age,
                .adult = profile.adult});
    });

    return app.run();
}
