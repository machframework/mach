#include <catch2/catch_test_macros.hpp>

#include <chrono>
#include <compare>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

#include <mach/App.hpp>
#include <mach/AppBuilder.hpp>
#include <mach/Context.hpp>
#include <mach/Json.hpp>
#include <mach/Reply.hpp>
#include <mach/controllers/ControllerBase.hpp>
#include <mach/controllers/ControllerBuilder.hpp>
#include <mach/results/Results.hpp>

#include <mach/detail/binding/BodyBinder.hpp>

#include "support/BeastHttpClient.hpp"

namespace
{
    using testing::http::BeastHttpClient;

    // -------------------------------------------------------------------------
    // Minimal API DTOs
    // -------------------------------------------------------------------------

    struct CreateUserRequest {
        std::string name;
        int age;
    };

    MACH_DEFINE_JSON(CreateUserRequest, name, age);

    struct UpdateScoreRequest {
        int score;
    };

    MACH_DEFINE_JSON(UpdateScoreRequest, score);

    // -------------------------------------------------------------------------
    // Controller DTO
    // -------------------------------------------------------------------------

    struct CreateControllerUserRequest {
        std::string name;
        int age;
    };

    MACH_DEFINE_JSON(CreateControllerUserRequest, name, age);

    class BindingController final : public mach::ControllerBase {
    public:
        inline static constexpr std::string_view route = "/binding/controller";

        static void configure(mach::ControllerBuilder<BindingController>& builder) {
            builder.mapGet("/no-body", &BindingController::noBody);

            builder.mapPost("/user", &BindingController::createUser);
        }

        mach::Reply<std::string> noBody() {
            return ok("no body");
        }

        mach::Reply<std::string> createUser(CreateControllerUserRequest request) {
            return ok(request.name + ":" + std::to_string(request.age));
        }
    };

    // -------------------------------------------------------------------------
    // Direct BodyBinder DTOs
    // -------------------------------------------------------------------------

    struct AddressDto {
        std::string city;
        std::string street;

        auto operator<=>(const AddressDto&) const = default;
    };

    MACH_DEFINE_JSON(AddressDto, city, street);

    struct UserDto {
        std::string name;
        AddressDto address;
    };

    MACH_DEFINE_JSON(UserDto, name, address);

    struct MoveOnlyDto {
        std::string name;
        int age;

        MoveOnlyDto() = default;

        MoveOnlyDto(const MoveOnlyDto&) = delete;
        MoveOnlyDto& operator=(const MoveOnlyDto&) = delete;

        MoveOnlyDto(MoveOnlyDto&&) = default;
        MoveOnlyDto& operator=(MoveOnlyDto&&) = default;
    };

    MACH_DEFINE_JSON(MoveOnlyDto, name, age);

    // -------------------------------------------------------------------------
    // Server
    // -------------------------------------------------------------------------

    class BindingServer {
    public:
        BindingServer() : m_app(mach::AppBuilder().addController<BindingController>().build()) {
            registerEndpoints();

            m_thread = std::thread([this] {
                m_app.run();
            });

            waitUntilReady();
        }

        ~BindingServer() {
            m_app.stop();

            if (m_thread.joinable()) {
                m_thread.join();
            }
        }

        BindingServer(const BindingServer&) = delete;
        BindingServer& operator=(const BindingServer&) = delete;

    private:
        void registerEndpoints() {
            m_app.mapGet("/binding/no-body", [] {
                return mach::ok("no body");
            });

            m_app.mapPost("/binding/user", [](CreateUserRequest request) {
                return mach::ok(request.name + ":" + std::to_string(request.age));
            });

            m_app.mapPost("/binding/integer", [](int value) {
                return mach::ok(value);
            });

            m_app.mapPost("/binding/string", [](std::string value) {
                return mach::ok(std::move(value));
            });

            m_app.mapPut("/binding/score", [](mach::Context&, UpdateScoreRequest request) {
                return mach::ok(request.score);
            });
        }

        void waitUntilReady() {
            constexpr int attempts = 100;

            BeastHttpClient client;

            for (int i = 0; i < attempts; ++i) {
                try {
                    (void)client.get("/binding/no-body");
                    return;
                } catch (...) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
            }

            throw std::runtime_error("Mach binding test server failed to start");
        }

        mach::App m_app;
        std::thread m_thread;
    };

    BindingServer& bindingServer() {
        static BindingServer server;
        return server;
    }

    BeastHttpClient& httpClient() {
        static BeastHttpClient client;
        return client;
    }

    void ensureServerRunning() {
        (void)bindingServer();
    }

    BeastHttpClient::Headers jsonHeaders() {
        return {{"Content-Type", "application/json"}};
    }
}

// -----------------------------------------------------------------------------
// Content-Type
// -----------------------------------------------------------------------------

TEST_CASE("Body binding validates Content-Type") {
    ensureServerRunning();

    SECTION("No body parameter does not require Content-Type") {
        const auto response = httpClient().get("/binding/no-body");

        REQUIRE(response.statusCode() == 200);
        REQUIRE(response.body() == "no body");
    }

    SECTION("No body parameter ignores unrelated Content-Type") {
        const auto response =
            httpClient().get("/binding/no-body", {{"Content-Type", "text/plain"}});

        REQUIRE(response.statusCode() == 200);
    }

    SECTION("application/json is accepted") {
        const auto response =
            httpClient().post("/binding/user", R"({"name":"Asaf","age":18})", jsonHeaders());

        REQUIRE(response.statusCode() == 200);
        REQUIRE(response.body() == "Asaf:18");
    }

    SECTION("application/json with charset is accepted") {
        const auto response = httpClient().post(
            "/binding/user",
            R"({"name":"Asaf","age":18})",
            {{"Content-Type", "application/json; charset=utf-8"}});

        REQUIRE(response.statusCode() == 200);
    }

    SECTION("Missing Content-Type is rejected") {
        const auto response = httpClient().post("/binding/user", R"({"name":"Asaf","age":18})");

        REQUIRE(response.statusCode() == 415);
    }

    SECTION("text/plain is rejected") {
        const auto response = httpClient().post(
            "/binding/user",
            R"({"name":"Asaf","age":18})",
            {{"Content-Type", "text/plain"}});

        REQUIRE(response.statusCode() == 415);
    }

    SECTION("application/xml is rejected") {
        const auto response = httpClient().post(
            "/binding/user",
            R"({"name":"Asaf","age":18})",
            {{"Content-Type", "application/xml"}});

        REQUIRE(response.statusCode() == 415);
    }

    SECTION("Structured JSON media type is rejected") {
        const auto response = httpClient().post(
            "/binding/user",
            R"({"name":"Asaf","age":18})",
            {{"Content-Type", "application/vnd.mach+json"}});

        REQUIRE(response.statusCode() == 415);
    }
}

// -----------------------------------------------------------------------------
// Invalid JSON
// -----------------------------------------------------------------------------

TEST_CASE("Body binding rejects invalid JSON bodies") {
    ensureServerRunning();

    SECTION("Malformed JSON") {
        const auto response =
            httpClient().post("/binding/user", R"({"name":"Asaf","age":18)", jsonHeaders());

        REQUIRE(response.statusCode() == 400);
    }

    SECTION("Incorrect field type") {
        const auto response = httpClient().post(
            "/binding/user",
            R"({"name":"Asaf","age":"eighteen"})",
            jsonHeaders());

        REQUIRE(response.statusCode() == 400);
    }

    SECTION("Missing required field") {
        const auto response =
            httpClient().post("/binding/user", R"({"name":"Asaf"})", jsonHeaders());

        REQUIRE(response.statusCode() == 400);
    }
}

// -----------------------------------------------------------------------------
// Primitive values
// -----------------------------------------------------------------------------

TEST_CASE("Body binding supports primitive JSON values") {
    ensureServerRunning();

    SECTION("Integer") {
        const auto response = httpClient().post("/binding/integer", "42", jsonHeaders());

        REQUIRE(response.statusCode() == 200);
        REQUIRE(response.body() == "42");
    }

    SECTION("Integer requires JSON Content-Type") {
        const auto response =
            httpClient().post("/binding/integer", "42", {{"Content-Type", "text/plain"}});

        REQUIRE(response.statusCode() == 415);
    }

    SECTION("JSON string") {
        const auto response = httpClient().post("/binding/string", R"("hello")", jsonHeaders());

        REQUIRE(response.statusCode() == 200);
        REQUIRE(response.body() == "hello");
    }

    SECTION("Unquoted string is invalid JSON") {
        const auto response = httpClient().post("/binding/string", "hello", jsonHeaders());

        REQUIRE(response.statusCode() == 400);
    }

    SECTION("Raw string with text/plain is rejected") {
        const auto response =
            httpClient().post("/binding/string", "hello", {{"Content-Type", "text/plain"}});

        REQUIRE(response.statusCode() == 415);
    }
}

// -----------------------------------------------------------------------------
// Context + body
// -----------------------------------------------------------------------------

TEST_CASE("Body binding works alongside Context parameters") {
    ensureServerRunning();

    SECTION("Valid JSON") {
        const auto response = httpClient().put("/binding/score", R"({"score":100})", jsonHeaders());

        REQUIRE(response.statusCode() == 200);
        REQUIRE(response.body() == "100");
    }

    SECTION("Unsupported Content-Type") {
        const auto response = httpClient().put(
            "/binding/score",
            R"({"score":100})",
            {{"Content-Type", "text/plain"}});

        REQUIRE(response.statusCode() == 415);
    }
}

// -----------------------------------------------------------------------------
// Controller binding
// -----------------------------------------------------------------------------

TEST_CASE("Controller action parameters use body binding") {
    ensureServerRunning();

    SECTION("No body parameter needs no Content-Type") {
        const auto response = httpClient().get("/binding/controller/no-body");

        REQUIRE(response.statusCode() == 200);
        REQUIRE(response.body() == "no body");
    }

    SECTION("DTO body is bound before invocation") {
        const auto response = httpClient().post(
            "/binding/controller/user",
            R"({"name":"Asaf","age":18})",
            jsonHeaders());

        REQUIRE(response.statusCode() == 200);
        REQUIRE(response.body() == "Asaf:18");
    }

    SECTION("Missing Content-Type is rejected") {
        const auto response =
            httpClient().post("/binding/controller/user", R"({"name":"Asaf","age":18})");

        REQUIRE(response.statusCode() == 415);
    }

    SECTION("Unsupported Content-Type is rejected") {
        const auto response = httpClient().post(
            "/binding/controller/user",
            R"({"name":"Asaf","age":18})",
            {{"Content-Type", "text/plain"}});

        REQUIRE(response.statusCode() == 415);
    }

    SECTION("Malformed JSON is rejected") {
        const auto response = httpClient().post(
            "/binding/controller/user",
            R"({"name":"Asaf","age":18)",
            jsonHeaders());

        REQUIRE(response.statusCode() == 400);
    }

    SECTION("Incorrect field type is rejected") {
        const auto response = httpClient().post(
            "/binding/controller/user",
            R"({"name":"Asaf","age":"eighteen"})",
            jsonHeaders());

        REQUIRE(response.statusCode() == 400);
    }

    SECTION("Missing field is rejected") {
        const auto response =
            httpClient().post("/binding/controller/user", R"({"name":"Asaf"})", jsonHeaders());

        REQUIRE(response.statusCode() == 400);
    }
}

// -----------------------------------------------------------------------------
// Structured values
// -----------------------------------------------------------------------------

TEST_CASE("BodyBinder supports structured object binding") {
    mach::detail::binding::BodyBinder binder;

    SECTION("Nested object") {
        const auto dto = binder.bind<UserDto>(
            R"({
                    "name": "Asaf",
                    "email": "ignored@example.com",
                    "address": {
                        "country": "Israel",
                        "city": "Gedera",
                        "street": "Herzl",
                        "houseNumber": 12
                    }
                })");

        REQUIRE(dto.name == "Asaf");
        REQUIRE(dto.address.city == "Gedera");
        REQUIRE(dto.address.street == "Herzl");
    }

    SECTION("Vector") {
        const auto addresses = binder.bind<std::vector<AddressDto>>(
            R"([
                    {
                        "city": "Gedera",
                        "street": "Herzl"
                    },
                    {
                        "city": "Tel Aviv",
                        "street": "Rothschild Boulevard"
                    },
                    {
                        "city": "Jerusalem",
                        "street": "Jaffa Street"
                    }
                ])");

        REQUIRE(addresses.size() == 3);
        REQUIRE(addresses.front().city == "Gedera");
    }

    SECTION("Set") {
        const auto addresses = binder.bind<std::set<AddressDto>>(
            R"([
                    {
                        "city": "Gedera",
                        "street": "Herzl"
                    },
                    {
                        "city": "Tel Aviv",
                        "street": "Rothschild Boulevard"
                    }
                ])");

        REQUIRE(addresses.size() == 2);
    }

    SECTION("Map") {
        const auto addresses = binder.bind<std::map<std::string, AddressDto>>(
            R"({
                    "home": {
                        "city": "Gedera",
                        "street": "Herzl"
                    },
                    "work": {
                        "city": "Tel Aviv",
                        "street": "Rothschild Boulevard"
                    },
                    "parents": {
                        "city": "Haifa",
                        "street": "HaNassi Boulevard"
                    }
                })");

        REQUIRE(addresses.size() == 3);
        REQUIRE(addresses.at("home").city == "Gedera");
    }
}

// -----------------------------------------------------------------------------
// Move-only DTOs
// -----------------------------------------------------------------------------

TEST_CASE("BodyBinder supports move-only DTOs") {
    mach::detail::binding::BodyBinder binder;

    auto dto = binder.bind<MoveOnlyDto>(
        R"({
                "name": "Asaf",
                "age": 18
            })");

    REQUIRE(dto.name == "Asaf");
    REQUIRE(dto.age == 18);
}
