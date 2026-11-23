#include <catch2/catch_test_macros.hpp>

#include <chrono>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>

#include <mach/App.hpp>
#include <mach/AppBuilder.hpp>
#include <mach/Json.hpp>
#include <mach/Reply.hpp>
#include <mach/binding/Body.hpp>
#include <mach/binding/Param.hpp>
#include <mach/binding/Query.hpp>
#include <mach/controllers/ControllerBase.hpp>
#include <mach/controllers/ControllerBuilder.hpp>

#include "support/BeastHttpClient.hpp"

namespace
{
    using testing::http::BeastHttpClient;

    struct UserDto {
        std::string name;
        int age;
    };

    MACH_DEFINE_JSON(UserDto, name, age);

    class ExplicitBindingController final : public mach::ControllerBase {
    public:
        inline static constexpr std::string_view route = "/controller-binding";

        static void configure(mach::ControllerBuilder<ExplicitBindingController>& builder) {
            builder.mapGet("/ready", &ExplicitBindingController::ready);

            builder.mapPost("/body/user", &ExplicitBindingController::bodyUser);
            builder.mapPost("/body/int", &ExplicitBindingController::bodyInt);
            builder.mapPost("/body/string", &ExplicitBindingController::bodyString);

            builder.mapGet("/param/int/{id}", &ExplicitBindingController::paramInt);
            builder.mapGet("/param/string/{name}", &ExplicitBindingController::paramString);
            builder.mapGet("/params/{userId}/{postId}", &ExplicitBindingController::multipleParams);

            builder.mapGet("/query/int", &ExplicitBindingController::queryInt);
            builder.mapGet("/query/string", &ExplicitBindingController::queryString);
            builder.mapGet("/query/optional", &ExplicitBindingController::queryOptional);
            builder.mapGet("/queries", &ExplicitBindingController::multipleQueries);

            builder.mapGet("/combined/{id}", &ExplicitBindingController::combined);
            builder.mapPost("/all/{id}", &ExplicitBindingController::all);

            builder.mapPost("/operators/body/int", &ExplicitBindingController::operatorBodyInt);
            builder.mapPost("/operators/body/user", &ExplicitBindingController::operatorBodyUser);
            builder.mapGet(
                "/operators/param/int/{id}",
                &ExplicitBindingController::operatorParamInt);
            builder.mapGet(
                "/operators/param/string/{name}",
                &ExplicitBindingController::operatorParamString);
            builder.mapGet("/operators/query/int", &ExplicitBindingController::operatorQueryInt);
            builder.mapGet(
                "/operators/query/optional",
                &ExplicitBindingController::operatorQueryOptional);

            builder.mapGet("/legacy/plain", &ExplicitBindingController::legacyPlain);
            builder.mapPost("/legacy/body", &ExplicitBindingController::legacyBody);

        }

        mach::Reply<std::string> ready() {
            return ok("ready");
        }

        mach::Reply<std::string> bodyUser(mach::Body<UserDto> body) {
            return ok(body.value().name + ":" + std::to_string(body.value().age));
        }

        mach::Reply<int> bodyInt(mach::Body<int> body) {
            return ok(body.value());
        }

        mach::Reply<std::string> bodyString(mach::Body<std::string> body) {
            return ok(body.value());
        }

        mach::Reply<int> paramInt(mach::Param<int, "id"> id) {
            return ok(id.value());
        }

        mach::Reply<std::string> paramString(mach::Param<std::string, "name"> name) {
            return ok(name.value());
        }

        mach::Reply<std::string> multipleParams(
            mach::Param<int, "userId"> userId,
            mach::Param<int, "postId"> postId) {
            return ok(std::to_string(userId.value()) + ":" + std::to_string(postId.value()));
        }

        mach::Reply<int> queryInt(mach::Query<int, "page"> page) {
            return ok(page.value());
        }

        mach::Reply<std::string> queryString(mach::Query<std::string, "term"> term) {
            return ok(term.value());
        }

        mach::Reply<std::string> queryOptional(mach::Query<std::optional<int>, "page"> page) {
            return ok(page.value().has_value() ? std::to_string(*page.value()) : "none");
        }

        mach::Reply<std::string> multipleQueries(
            mach::Query<int, "page"> page,
            mach::Query<std::string, "sort"> sort) {
            return ok(std::to_string(page.value()) + ":" + sort.value());
        }

        mach::Reply<std::string> combined(
            mach::Param<int, "id"> id,
            mach::Query<int, "page"> page) {
            return ok(std::to_string(*id) + ":" + std::to_string(*page));
        }

        mach::Reply<std::string> all(
            mach::Query<int, "page"> page,
            mach::Body<UserDto> body,
            mach::Param<int, "id"> id) {
            return ok(std::to_string(*id) + ":" + body->name + ":" + std::to_string(*page));
        }

        mach::Reply<int> operatorBodyInt(mach::Body<int> number) {
            return ok(*number * 2);
        }

        mach::Reply<std::string> operatorBodyUser(mach::Body<UserDto> user) {
            return ok(user->name + ":" + std::to_string(user->age));
        }

        mach::Reply<int> operatorParamInt(mach::Param<int, "id"> id) {
            return ok(*id);
        }

        mach::Reply<std::string> operatorParamString(mach::Param<std::string, "name"> name) {
            return ok(std::to_string(name->size()));
        }

        mach::Reply<int> operatorQueryInt(mach::Query<int, "page"> page) {
            return ok(*page);
        }

        mach::Reply<std::string> operatorQueryOptional(
            mach::Query<std::optional<int>, "page"> page) {
            return ok(page->has_value() ? std::to_string(page->value()) : "none");
        }

        mach::Reply<std::string> legacyPlain() {
            return ok("plain");
        }

        mach::Reply<std::string> legacyBody(UserDto user) {
            return ok(user.name);
        }
    };

    class ControllerBindingServer {
    public:
        ControllerBindingServer()
            : m_app(mach::AppBuilder().addController<ExplicitBindingController>().build()) {
            m_thread = std::thread([this] {
                m_app.run();
            });
            waitUntilReady();
        }

        ~ControllerBindingServer() {
            m_app.stop();
            if (m_thread.joinable()) {
                m_thread.join();
            }
        }

        ControllerBindingServer(const ControllerBindingServer&) = delete;
        ControllerBindingServer& operator=(const ControllerBindingServer&) = delete;

    private:
        void waitUntilReady() {
            BeastHttpClient client;
            for (int attempt = 0; attempt < 100; ++attempt) {
                try {
                    const auto response = client.get("/controller-binding/ready");
                    if (response.statusCode() == 200 && response.body() == "ready") {
                        return;
                    }
                } catch (...) {
                    // The server may not be listening yet.
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
            throw std::runtime_error("Controller binding test server failed to start");
        }

        mach::App m_app;
        std::thread m_thread;
    };

    void ensureServerRunning() {
        static ControllerBindingServer server;
        (void)server;
    }

    BeastHttpClient& client() {
        static BeastHttpClient instance;
        return instance;
    }

    BeastHttpClient::Headers jsonHeaders() {
        return {{"Content-Type", "application/json"}};
    }
}

TEST_CASE("Controller explicit body binding") {
    ensureServerRunning();

    SECTION("DTO") {
        const auto response = client().post(
            "/controller-binding/body/user",
            R"({"name":"Alice","age":30})",
            jsonHeaders());
        REQUIRE(response.statusCode() == 200);
        REQUIRE(response.body() == "Alice:30");
    }

    SECTION("Integer") {
        const auto response = client().post("/controller-binding/body/int", "42", jsonHeaders());
        REQUIRE(response.statusCode() == 200);
        REQUIRE(response.body() == "42");
    }

    SECTION("JSON string") {
        const auto response =
            client().post("/controller-binding/body/string", R"("hello")", jsonHeaders());
        REQUIRE(response.statusCode() == 200);
        REQUIRE(response.body() == "hello");
    }

    SECTION("Malformed JSON") {
        const auto response = client().post(
            "/controller-binding/body/user",
            R"({"name":"Alice","age":30)",
            jsonHeaders());
        REQUIRE(response.statusCode() == 400);
    }

    SECTION("Incorrect DTO field type") {
        const auto response = client().post(
            "/controller-binding/body/user",
            R"({"name":"Alice","age":"invalid"})",
            jsonHeaders());
        REQUIRE(response.statusCode() == 400);
    }

    SECTION("Unsupported Content-Type") {
        const auto response =
            client().post("/controller-binding/body/int", "42", {{"Content-Type", "text/plain"}});
        REQUIRE(response.statusCode() == 415);
    }

    SECTION("Missing Content-Type") {
        const auto response = client().post("/controller-binding/body/int", "42");
        REQUIRE(response.statusCode() == 415);
    }
}

TEST_CASE("Controller explicit route parameter binding") {
    ensureServerRunning();

    SECTION("Integer") {
        const auto response = client().get("/controller-binding/param/int/42");
        REQUIRE(response.statusCode() == 200);
        REQUIRE(response.body() == "42");
    }

    SECTION("String") {
        const auto response = client().get("/controller-binding/param/string/Alice");
        REQUIRE(response.statusCode() == 200);
        REQUIRE(response.body() == "Alice");
    }

    SECTION("Invalid integer") {
        const auto response = client().get("/controller-binding/param/int/not-a-number");
        REQUIRE(response.statusCode() == 400);
    }
}

TEST_CASE("Controller explicit query parameter binding") {
    ensureServerRunning();

    SECTION("Integer") {
        const auto response = client().get("/controller-binding/query/int?page=12");
        REQUIRE(response.statusCode() == 200);
        REQUIRE(response.body() == "12");
    }

    SECTION("String") {
        const auto response = client().get("/controller-binding/query/string?term=hello");
        REQUIRE(response.statusCode() == 200);
        REQUIRE(response.body() == "hello");
    }

    SECTION("Missing required parameter") {
        const auto response = client().get("/controller-binding/query/int");
        REQUIRE(response.statusCode() == 400);
    }

    SECTION("Invalid integer") {
        const auto response = client().get("/controller-binding/query/int?page=invalid");
        REQUIRE(response.statusCode() == 400);
    }

    SECTION("Missing optional parameter") {
        const auto response = client().get("/controller-binding/query/optional");
        REQUIRE(response.statusCode() == 200);
        REQUIRE(response.body() == "none");
    }

    SECTION("Present optional parameter") {
        const auto response = client().get("/controller-binding/query/optional?page=7");
        REQUIRE(response.statusCode() == 200);
        REQUIRE(response.body() == "7");
    }
}

TEST_CASE("Controller combines explicit binding sources") {
    ensureServerRunning();

    SECTION("Route and query") {
        const auto response = client().get("/controller-binding/combined/42?page=3");
        REQUIRE(response.statusCode() == 200);
        REQUIRE(response.body() == "42:3");
    }

    SECTION("Query, body and route in noncanonical order") {
        const auto response = client().post(
            "/controller-binding/all/42?page=3",
            R"({"name":"Alice","age":30})",
            jsonHeaders());
        REQUIRE(response.statusCode() == 200);
        REQUIRE(response.body() == "42:Alice:3");
    }

    SECTION("Multiple route parameters") {
        const auto response = client().get("/controller-binding/params/42/7");
        REQUIRE(response.statusCode() == 200);
        REQUIRE(response.body() == "42:7");
    }

    SECTION("Multiple query parameters") {
        const auto response = client().get("/controller-binding/queries?page=3&sort=desc");
        REQUIRE(response.statusCode() == 200);
        REQUIRE(response.body() == "3:desc");
    }

    SECTION("Combined binding with invalid query") {
        const auto response = client().post(
            "/controller-binding/all/42?page=invalid",
            R"({"name":"Alice","age":30})",
            jsonHeaders());
        REQUIRE(response.statusCode() == 400);
    }
}

TEST_CASE("Controller binding wrapper access operators") {
    ensureServerRunning();

    SECTION("Body integer dereference") {
        const auto response =
            client().post("/controller-binding/operators/body/int", "21", jsonHeaders());
        REQUIRE(response.statusCode() == 200);
        REQUIRE(response.body() == "42");
    }

    SECTION("Body DTO member access") {
        const auto response = client().post(
            "/controller-binding/operators/body/user",
            R"({"name":"Alice","age":30})",
            jsonHeaders());
        REQUIRE(response.statusCode() == 200);
        REQUIRE(response.body() == "Alice:30");
    }

    SECTION("Param integer dereference") {
        const auto response = client().get("/controller-binding/operators/param/int/42");
        REQUIRE(response.statusCode() == 200);
        REQUIRE(response.body() == "42");
    }

    SECTION("Param string member access") {
        const auto response = client().get("/controller-binding/operators/param/string/Alice");
        REQUIRE(response.statusCode() == 200);
        REQUIRE(response.body() == "5");
    }

    SECTION("Query integer dereference") {
        const auto response = client().get("/controller-binding/operators/query/int?page=12");
        REQUIRE(response.statusCode() == 200);
        REQUIRE(response.body() == "12");
    }

    SECTION("Query optional member access: present") {
        const auto response = client().get("/controller-binding/operators/query/optional?page=7");
        REQUIRE(response.statusCode() == 200);
        REQUIRE(response.body() == "7");
    }

    SECTION("Query optional member access: missing") {
        const auto response = client().get("/controller-binding/operators/query/optional");
        REQUIRE(response.statusCode() == 200);
        REQUIRE(response.body() == "none");
    }
}

TEST_CASE("Controller preserves legacy action signatures") {
    ensureServerRunning();

    SECTION("Zero-argument action") {
        const auto response = client().get("/controller-binding/legacy/plain");
        REQUIRE(response.statusCode() == 200);
        REQUIRE(response.body() == "plain");
    }

    SECTION("Implicit JSON body action") {
        const auto response = client().post(
            "/controller-binding/legacy/body",
            R"({"name":"Alice","age":30})",
            jsonHeaders());
        REQUIRE(response.statusCode() == 200);
        REQUIRE(response.body() == "Alice");
    }
}
