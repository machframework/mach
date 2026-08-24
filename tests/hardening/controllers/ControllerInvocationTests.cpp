#include <iostream>
#include <string>
#include <string_view>

#include <mach/App.hpp>
#include <mach/AppBuilder.hpp>
#include <mach/Json.hpp>
#include <mach/controllers/ControllerBase.hpp>
#include <mach/controllers/ControllerBuilder.hpp>

#include "Testing.hpp"

namespace
{
    struct CreateUserRequest {
        std::string name;
        int age;
    };

    MACH_DEFINE_JSON(CreateUserRequest, name, age);

    class InvocationService {
    public:
        [[nodiscard]]
        std::string message() const {
            return "dependency injected";
        }
    };

    class InvocationController final : public mach::ControllerBase {
    public:
        inline static constexpr std::string_view route = "/controller-invocation";

        explicit InvocationController(InvocationService& service) : m_service(service) {}

        static void configure(mach::ControllerBuilder<InvocationController>& builder) {
            builder.mapGet("/plain", &InvocationController::plain);

            builder.mapGet("/dependency", &InvocationController::dependency);

            builder.mapGet("/context", &InvocationController::contextParameter);

            builder.mapGet("/base-context", &InvocationController::baseContext);

            builder.mapGet("/users/{id:int}", &InvocationController::routeParameter);

            builder.mapPost("/body", &InvocationController::body);

            builder.mapGet("/created", &InvocationController::createdResult);
        }

        mach::Reply<std::string> plain() {
            return ok("plain action invoked");
        }

        mach::Reply<std::string> dependency() {
            return ok(m_service.message());
        }

        mach::Reply<std::string> contextParameter() {
            const int id = request().routeParam<int>("id");

            return ok("context parameter received: " + std::to_string(id));
        }

        mach::Reply<std::string> baseContext() {
            return ok("controller base context assigned");
        }

        mach::Reply<std::string> routeParameter() {
            const int id = request().routeParam<int>("id");

            return ok("route parameter: " + std::to_string(id));
        }

        mach::Reply<CreateUserRequest> body(CreateUserRequest request) {
            return ok(std::move(request));
        }

        mach::Reply<std::string> createdResult() {
            return created("created result");
        }

    private:
        InvocationService& m_service;
    };
}

int main() {
    auto app = mach::AppBuilder()
                   .addScoped<InvocationService>()
                   .addController<InvocationController, InvocationService>()
                   .build();

    std::cout << "Controller invocation test server started.\n\n"
              << "Send these requests manually:\n\n"

              << "GET /controller-invocation/plain\n"
              << "Expected: 200, body: plain action invoked\n\n"

              << "GET /controller-invocation/dependency\n"
              << "Expected: 200, body: dependency injected\n\n"

              << "GET /controller-invocation/base-context\n"
              << "Expected: 200, body: controller base context assigned\n\n"

              << "GET /controller-invocation/users/42\n"
              << "Expected: 200, body: route parameter: 42\n\n"

              << "POST /controller-invocation/body\n"
              << "Content-Type: application/json\n"
              << R"(Body: {"name":"Asaf","age":17})"
              << "\n"
              << R"(Expected: 200, JSON body containing {"name":"Asaf","age":17})"
              << "\n\n"

              << "GET /controller-invocation/created\n"
              << "Expected: 201, body: created result\n\n";

    return app.run();
}