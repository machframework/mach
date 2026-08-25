#include <catch2/catch_test_macros.hpp>

#include <chrono>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>

#include <mach/App.hpp>
#include <mach/AppBuilder.hpp>
#include <mach/Json.hpp>
#include <mach/Reply.hpp>
#include <mach/controllers/ControllerBase.hpp>
#include <mach/controllers/ControllerBuilder.hpp>

#include <mach/detail/di/Container.hpp>
#include <mach/detail/di/Scope.hpp>
#include <mach/detail/di/ServiceLifetime.hpp>

#include "support/BeastHttpClient.hpp"

namespace
{
    namespace di = mach::detail::di;

    // -------------------------------------------------------------------------
    // Controller discovery
    // -------------------------------------------------------------------------

    class FirstController final : public mach::ControllerBase {
    public:
        inline static constexpr std::string_view route = "/first";

        static void reset() noexcept {
            s_configurationCount = 0;
        }

        [[nodiscard]]
        static int configurationCount() noexcept {
            return s_configurationCount;
        }

        static void configure(mach::ControllerBuilder<FirstController>& builder) {
            ++s_configurationCount;

            builder.mapGet("/", &FirstController::index);
        }

        mach::Reply<> index() {
            return noContent();
        }

    private:
        inline static int s_configurationCount = 0;
    };

    class SecondController final : public mach::ControllerBase {
    public:
        inline static constexpr std::string_view route = "/second";

        static void reset() noexcept {
            s_configurationCount = 0;
        }

        [[nodiscard]]
        static int configurationCount() noexcept {
            return s_configurationCount;
        }

        static void configure(mach::ControllerBuilder<SecondController>& builder) {
            ++s_configurationCount;

            builder.mapGet("/", &SecondController::index);
            builder.mapGet("/details", &SecondController::details);
        }

        mach::Reply<> index() {
            return noContent();
        }

        mach::Reply<> details() {
            return noContent();
        }

    private:
        inline static int s_configurationCount = 0;
    };

    class UnregisteredController final : public mach::ControllerBase {
    public:
        inline static constexpr std::string_view route = "/unregistered";

        static void reset() noexcept {
            s_configurationCount = 0;
        }

        [[nodiscard]]
        static int configurationCount() noexcept {
            return s_configurationCount;
        }

        static void configure(mach::ControllerBuilder<UnregisteredController>& builder) {
            ++s_configurationCount;

            builder.mapGet("/", &UnregisteredController::index);
        }

        mach::Reply<> index() {
            return noContent();
        }

    private:
        inline static int s_configurationCount = 0;
    };

    void resetDiscoveryControllers() noexcept {
        FirstController::reset();
        SecondController::reset();
        UnregisteredController::reset();
    }

    // -------------------------------------------------------------------------
    // Controller resolution
    // -------------------------------------------------------------------------

    struct ScopedService {};

    class ResolvedController final : public mach::ControllerBase {
    public:
        inline static constexpr std::string_view route = "/resolved";

        explicit ResolvedController(ScopedService& service) : service(service) {}

        static void configure(mach::ControllerBuilder<ResolvedController>&) {}

        ScopedService& service;
    };

    struct Logger {};

    class MultipleConstructorController final : public mach::ControllerBase {
    public:
        MultipleConstructorController(ScopedService&) : constructorUsed(1) {}

        MultipleConstructorController(ScopedService&, Logger&) : constructorUsed(2) {}

        int constructorUsed;
    };

    // -------------------------------------------------------------------------
    // Runtime controller invocation
    // -------------------------------------------------------------------------

    class InvocationService {
    public:
        [[nodiscard]]
        std::string message() const {
            return "dependency injected";
        }
    };

    struct Person {
        std::string name;
        int age;
        bool male;
    };

    MACH_DEFINE_JSON(Person, name, age, male);

    class InvocationController final : public mach::ControllerBase {
    public:
        inline static constexpr std::string_view route = "/invocation";

        explicit InvocationController(InvocationService& service) : m_service(service) {}

        static void configure(mach::ControllerBuilder<InvocationController>& builder) {
            builder.mapGet("/plain", &InvocationController::plain);

            builder.mapGet("/dependency", &InvocationController::dependency);

            builder.mapGet("/users/{id:int}", &InvocationController::routeParameter);

            builder.mapGet("/context", &InvocationController::context);

            builder.mapPost("/body", &InvocationController::body);

            builder.mapGet("/created", &InvocationController::createdResult);
        }

        mach::Reply<std::string> plain() {
            return ok("plain action invoked");
        }

        mach::Reply<std::string> dependency() {
            return ok(m_service.message());
        }

        mach::Reply<int> routeParameter() {
            return ok(request().routeParam<int>("id"));
        }

        mach::Reply<std::string> context() {
            return ok(std::string(request().target()));
        }

        mach::Reply<int> body(Person person) {
            return ok(person.age);
        }

        mach::Reply<std::string> createdResult() {
            return created("created result");
        }

    private:
        InvocationService& m_service;
    };

    // -------------------------------------------------------------------------
    // Route mapping overloads
    // -------------------------------------------------------------------------

    class MappingController final : public mach::ControllerBase {
    public:
        inline static constexpr std::string_view route = "/mapping";

        static void configure(mach::ControllerBuilder<MappingController>& builder) {
            builder.mapGet("/age", &MappingController::explicitRoute);

            builder.mapGet(&MappingController::implicitRoute);
        }

        mach::Reply<int> explicitRoute() {
            return ok(5);
        }

        mach::Reply<int> implicitRoute() {
            return ok(10);
        }
    };

    // -------------------------------------------------------------------------
    // Test server
    // -------------------------------------------------------------------------

    class ControllerServer {
    public:
        ControllerServer()
            : m_app(
                  mach::AppBuilder()
                      .addScoped<InvocationService>()
                      .addController<InvocationController, InvocationService>()
                      .build()) {
            m_thread = std::thread([this] {
                m_app.run();
            });

            waitUntilReady();
        }

        ~ControllerServer() {
            m_app.stop();

            if (m_thread.joinable()) {
                m_thread.join();
            }
        }

        ControllerServer(const ControllerServer&) = delete;
        ControllerServer& operator=(const ControllerServer&) = delete;

    private:
        void waitUntilReady() {
            constexpr int attempts = 100;

            testing::http::BeastHttpClient client;

            for (int i = 0; i < attempts; ++i) {
                try {
                    (void)client.get("/invocation/plain");
                    return;
                } catch (...) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
            }

            throw std::runtime_error("Mach controller test server failed to start");
        }

        mach::App m_app;
        std::thread m_thread;
    };

    ControllerServer& controllerServer() {
        static ControllerServer server;
        return server;
    }

    void ensureServerRunning() {
        (void)controllerServer();
    }

    testing::http::BeastHttpClient& httpClient() {
        static testing::http::BeastHttpClient client;
        return client;
    }
}

// -----------------------------------------------------------------------------
// Discovery
// -----------------------------------------------------------------------------

TEST_CASE("Controllers are discovered during application build") {
    resetDiscoveryControllers();

    SECTION("Registration does not configure controller") {
        mach::AppBuilder builder;

        builder.addController<FirstController>();

        REQUIRE(FirstController::configurationCount() == 0);
    }

    SECTION("Registered controller is configured exactly once") {
        mach::AppBuilder builder;

        builder.addController<FirstController>();

        [[maybe_unused]]
        auto app = builder.build();

        REQUIRE(FirstController::configurationCount() == 1);
    }

    SECTION("Multiple registered controllers are configured") {
        mach::AppBuilder builder;

        builder.addController<FirstController>();
        builder.addController<SecondController>();

        [[maybe_unused]]
        auto app = builder.build();

        REQUIRE(FirstController::configurationCount() == 1);
        REQUIRE(SecondController::configurationCount() == 1);
    }

    SECTION("Unregistered controllers are ignored") {
        mach::AppBuilder builder;

        builder.addController<FirstController>();
        builder.addController<SecondController>();

        [[maybe_unused]]
        auto app = builder.build();

        REQUIRE(UnregisteredController::configurationCount() == 0);
    }

    SECTION("Controller with multiple actions is configured once") {
        mach::AppBuilder builder;

        builder.addController<SecondController>();

        [[maybe_unused]]
        auto app = builder.build();

        REQUIRE(SecondController::configurationCount() == 1);
    }
}

// -----------------------------------------------------------------------------
// Resolution
// -----------------------------------------------------------------------------

TEST_CASE("Controllers are resolved with their declared dependencies") {
    di::Container container;

    container.addService<ScopedService>(di::ServiceLifetime::Scoped);

    container.addService<ResolvedController, ScopedService>(
        di::ServiceLifetime::Transient,
        di::ServiceAccess::Internal);

    container.finalizeRegistrations();

    SECTION("Controller is transient") {
        auto scope = container.createScope();

        auto& first = scope.resolve<ResolvedController>();
        auto& second = scope.resolve<ResolvedController>();

        REQUIRE(&first != &second);
    }

    SECTION("Scoped dependency is shared within a scope") {
        auto scope = container.createScope();

        auto& first = scope.resolve<ResolvedController>();
        auto& second = scope.resolve<ResolvedController>();

        REQUIRE(&first.service == &second.service);
    }

    SECTION("Scoped dependency differs between scopes") {
        auto firstScope = container.createScope();
        auto secondScope = container.createScope();

        auto& first = firstScope.resolve<ResolvedController>();
        auto& second = secondScope.resolve<ResolvedController>();

        REQUIRE(&first.service != &second.service);
    }
}

TEST_CASE("Controller resolution uses the declared constructor") {
    di::Container container;

    container.addService<ScopedService>(di::ServiceLifetime::Scoped);

    container.addService<Logger>(di::ServiceLifetime::Singleton);

    container.addService<MultipleConstructorController, ScopedService, Logger>(
        di::ServiceLifetime::Transient,
        di::ServiceAccess::Internal);

    container.finalizeRegistrations();

    auto scope = container.createScope();

    REQUIRE(scope.resolve<MultipleConstructorController>().constructorUsed == 2);
}

// -----------------------------------------------------------------------------
// Invocation
// -----------------------------------------------------------------------------

TEST_CASE("Controller actions are invoked through HTTP") {
    ensureServerRunning();

    SECTION("Plain action") {
        const auto response = httpClient().get("/invocation/plain");

        REQUIRE(response.statusCode() == 200);
        REQUIRE(response.body() == "plain action invoked");
    }

    SECTION("Controller dependency is injected") {
        const auto response = httpClient().get("/invocation/dependency");

        REQUIRE(response.statusCode() == 200);
        REQUIRE(response.body() == "dependency injected");
    }

    SECTION("Created result becomes 201 response") {
        const auto response = httpClient().get("/invocation/created");

        REQUIRE(response.statusCode() == 201);
        REQUIRE(response.body() == "created result");
    }
}

TEST_CASE("ControllerBase receives the request context") {
    ensureServerRunning();

    const auto response = httpClient().get("/invocation/context");

    REQUIRE(response.statusCode() == 200);
    REQUIRE(response.body() == "/invocation/context");
}

TEST_CASE("Controller actions can access route parameters") {
    ensureServerRunning();

    const auto response = httpClient().get("/invocation/users/42");

    REQUIRE(response.statusCode() == 200);
    REQUIRE(response.body() == "42");
}

TEST_CASE("Controller action parameters are bound from request bodies") {
    ensureServerRunning();

    const auto response =
        httpClient().post("/invocation/body", R"({"name":"Asaf","age":17,"male":true})");

    REQUIRE(response.statusCode() == 200);
    REQUIRE(response.body() == "17");
}

// -----------------------------------------------------------------------------
// Mapping
// -----------------------------------------------------------------------------

TEST_CASE("ControllerBuilder supports route mapping overloads") {
    mach::AppBuilder builder;

    builder.addController<MappingController>();

    REQUIRE_NOTHROW(builder.build());
}
