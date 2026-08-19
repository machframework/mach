#include <string>
#include <string_view>

#include <mach/controllers.hpp>
#include <mach/exceptions.hpp>

class UserService {
public:
    std::string getUser(int id) const {
        if (id <= 0) {
            throw mach::BadRequestException();
        }

        if (id != 42) {
            throw mach::NotFoundException();
        }

        return "User 42";
    }
};

class UserController : public mach::ControllerBase {
public:
    explicit UserController(UserService& users) : m_users(users) {}

    inline static constexpr std::string_view route = "/users";

    mach::Reply<std::string> getUser() {
        const int id = request().routeParam<int>("id");
        return ok(m_users.getUser(id));
    }

    static void configure(mach::ControllerBuilder<UserController>& routes) {
        routes.mapGet("/{id:int}", &UserController::getUser);
    }

private:
    UserService& m_users;
};

int main() {
    auto builder = mach::AppBuilder();

    builder.addScoped<UserService>();
    builder.addController<UserController, UserService>();

    auto app = builder.build();
    return app.run();
}
