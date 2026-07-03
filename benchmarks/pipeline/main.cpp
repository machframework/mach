#include <iostream>
#include <string>
#include <string_view>
#include <thread>

#include <mach/AppBuilder.hpp>
#include <mach/controllers/ControllerBase.hpp>
#include <mach/controllers/ControllerBuilder.hpp>
#include <mach/Context.hpp>
#include <mach/results/Reply.hpp>

#include "server/BeastSession.hpp"

struct RequestIdService {
    std::string make() const {
        return "req-123";
    }
};

struct UserService {
    std::string getUser(std::string_view id) const {
        return std::string("user:") + std::string(id);
    }

    int calculateAge(int year) const {
        return 2026 - year;
    }
};

struct HeaderMiddleware {
    void invoke(mach::Context& ctx, auto next) {
        ctx.response.setHeader("X-Mach-Benchmark", "middleware");
        next();
    }
};

struct TouchMiddleware {
    void invoke(mach::Context& ctx, auto next) {
        ctx.response.setHeader("X-Touched", "1");
        next();
    }
};

class HomeController : public mach::ControllerBase {
public:
    inline static std::string route = "/runtime";

    explicit HomeController(RequestIdService& requestIds)
        : m_requestIds(requestIds) {}

    static void configure(mach::ControllerBuilder<HomeController>& builder) {
        builder.mapGet("/controller/static", &HomeController::staticLookup);
        builder.mapGet("/controller/request-id", &HomeController::requestId);
        builder.mapGet("/controller/precedence/me", &HomeController::precedenceStatic);
        builder.mapGet("/controller/precedence/{name}", &HomeController::precedenceParam);
    }

    mach::Reply<std::string> staticLookup() {
        return ok("controller-static");
    }

    mach::Reply<std::string> requestId() {
        return ok(m_requestIds.make());
    }

    mach::Reply<std::string> precedenceStatic() {
        return ok("me");
    }

    mach::Reply<std::string> precedenceParam() {
        return ok("param");
    }

private:
    RequestIdService& m_requestIds;
};

class UsersController : public mach::ControllerBase {
public:
    inline static std::string route = "/runtime/users";

    explicit UsersController(UserService& users)
        : m_users(users) {}

    static void configure(mach::ControllerBuilder<UsersController>& builder) {
        builder.mapGet("/{id}", &UsersController::getUser);
        builder.mapGet("/{userId}/posts/{postId}", &UsersController::getUserPost);
        builder.mapGet("/birth-year/{year:int}", &UsersController::birthYear);
    }

    mach::Reply<std::string> getUser() {
        const auto id = context->request.routeParam("id");
        return ok(m_users.getUser(id));
    }

    mach::Reply<std::string> getUserPost() {
        const auto userId = context->request.routeParam("userId");
        const auto postId = context->request.routeParam("postId");

        return ok(std::string(userId) + ":" + std::string(postId));
    }

    mach::Reply<std::string> birthYear() {
        const auto yearText = context->request.routeParam("year");
        const int year = std::stoi(std::string(yearText));

        return ok(std::to_string(m_users.calculateAge(year)));
    }

private:
    UserService& m_users;
};

int main() {
    auto builder = mach::AppBuilder("127.0.0.1", 3143, 16);

    builder.addSingleton<RequestIdService>();
    builder.addScoped<UserService>();

    builder.use<HeaderMiddleware>();
    builder.use<TouchMiddleware>();

    builder.addController<HomeController, RequestIdService>();
    builder.addController<UsersController, UserService>();

    auto app = builder.build();

    app.mapController<HomeController>();
    app.mapController<UsersController>();

	std::jthread diagnosticThread([]() {
		while (true) {
			std::this_thread::sleep_for(std::chrono::seconds(5));
            std::cout
                << "\n===== Lifetime Diagnostics =====\n"
                << "Scope\n"
                << "  Alive:     " << mach::detail::di::Scope::aliveCount() << '\n'
                << "  Created:   " << mach::detail::di::Scope::createdCount() << '\n'
                << '\n'
                << "Session\n"
                << "  Alive:     " << mach::detail::server::BeastSession::aliveCount() << '\n'
                << "  Created:   " << mach::detail::server::BeastSession::createdCount() << '\n'
                << '\n'
                << "Middleware Pipeline\n"
                << "  Alive:     " << mach::detail::middleware::MiddlewarePipeline::aliveCount() << '\n'
                << "  Created:   " << mach::detail::middleware::MiddlewarePipeline::createdCount() << '\n'
                << "===============================\n";
		}
		});

    return app.run();
}
