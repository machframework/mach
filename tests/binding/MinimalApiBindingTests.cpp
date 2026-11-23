#include <catch2/catch_test_macros.hpp>

#include <chrono>
#include <concepts>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>
#include <type_traits>
#include <utility>

#include <mach/App.hpp>
#include <mach/AppBuilder.hpp>
#include <mach/Context.hpp>
#include <mach/Json.hpp>
#include <mach/Reply.hpp>
#include <mach/results/Results.hpp>

#include <mach/binding/Body.hpp>
#include <mach/binding/Param.hpp>
#include <mach/binding/Query.hpp>

#include "support/BeastHttpClient.hpp"

namespace
{
    using testing::http::BeastHttpClient;

    struct UserDto {
        std::string name;
        int age;
    };

    MACH_DEFINE_JSON(UserDto, name, age);

    template <typename Wrapper, typename Value>
    constexpr bool hasReadOnlyAccess =
        std::same_as<decltype(*std::declval<Wrapper&>()), const Value&> &&
        std::same_as<decltype(std::declval<Wrapper&>().operator->()), const Value*> &&
        std::same_as<decltype(std::declval<const Wrapper&>().value()), const Value&> &&
        std::same_as<decltype(*std::declval<const Wrapper&>()), const Value&> &&
        std::same_as<decltype(std::declval<const Wrapper&>().operator->()), const Value*>;

    static_assert(hasReadOnlyAccess<mach::Body<int>, int>);
    static_assert(hasReadOnlyAccess<mach::Body<UserDto>, UserDto>);
    static_assert(hasReadOnlyAccess<mach::Param<int, "id">, int>);
    static_assert(hasReadOnlyAccess<mach::Param<std::string, "name">, std::string>);
    static_assert(hasReadOnlyAccess<mach::Query<int, "page">, int>);
    static_assert(hasReadOnlyAccess<mach::Query<std::optional<int>, "page">, std::optional<int>>);

    class ExplicitBindingServer {
    public:
        ExplicitBindingServer() : m_app(mach::AppBuilder().build()) {
            registerEndpoints();
            m_thread = std::thread([this] {
                m_app.run();
            });
            waitUntilReady();
        }

        ~ExplicitBindingServer() {
            m_app.stop();
            if (m_thread.joinable()) {
                m_thread.join();
            }
        }

        ExplicitBindingServer(const ExplicitBindingServer&) = delete;
        ExplicitBindingServer& operator=(const ExplicitBindingServer&) = delete;

    private:
        void registerEndpoints() {
            m_app.mapGet("/explicit/ready", [] {
                return mach::ok("ready");
            });

            m_app.mapPost("/explicit/body/user", [](mach::Body<UserDto> body) {
                return mach::ok(body.value().name + ":" + std::to_string(body.value().age));
            });

            m_app.mapPost("/explicit/body/int", [](mach::Body<int> body) {
                return mach::ok(body.value());
            });

            m_app.mapPost("/explicit/body/string", [](mach::Body<std::string> body) {
                return mach::ok(body.value());
            });

            m_app.mapGet("/explicit/param/int/{id}", [](mach::Param<int, "id"> id) {
                return mach::ok(id.value());
            });

            m_app.mapGet(
                "/explicit/param/string/{name}",
                [](mach::Param<std::string, "name"> name) {
                    return mach::ok(name.value());
                });

            m_app.mapGet("/explicit/query/int", [](mach::Query<int, "page"> page) {
                return mach::ok(page.value());
            });

            m_app.mapGet("/explicit/query/string", [](mach::Query<std::string, "term"> term) {
                return mach::ok(term.value());
            });

            m_app.mapGet(
                "/explicit/query/optional",
                [](mach::Query<std::optional<int>, "page"> page) {
                    return mach::ok(
                        page.value().has_value() ? std::to_string(*page.value())
                                                 : std::string("none"));
                });

            m_app.mapGet(
                "/explicit/combined/{id}",
                [](mach::Param<int, "id"> id, mach::Query<int, "page"> page) {
                    return mach::ok(
                        std::to_string(id.value()) + ":" + std::to_string(page.value()));
                });

            m_app.mapGet(
                "/explicit/params/{userId}/{postId}",
                [](mach::Param<int, "userId"> userId, mach::Param<int, "postId"> postId) {
                    return mach::ok(
                        std::to_string(userId.value()) + ":" + std::to_string(postId.value()));
                });

            m_app.mapGet(
                "/explicit/queries",
                [](mach::Query<int, "page"> page, mach::Query<std::string, "sort"> sort) {
                    return mach::ok(std::to_string(page.value()) + ":" + sort.value());
                });

            m_app.mapPost(
                "/explicit/all/{id}",
                [](mach::Query<int, "page"> page,
                   mach::Body<UserDto> body,
                   mach::Param<int, "id"> id) {
                    return mach::ok(
                        std::to_string(id.value()) + ":" + body.value().name + ":" +
                        std::to_string(page.value()));
                });

            m_app.mapGet(
                "/explicit/context/first/{id}",
                [](mach::Context& context, mach::Param<int, "id"> id) {
                    return mach::ok(
                        std::to_string(id.value()) + ":" +
                        std::string(context.request.routeParam("id")));
                });

            m_app.mapGet(
                "/explicit/context/middle/{id}",
                [](mach::Param<int, "id"> id,
                   const mach::Context& context,
                   mach::Query<int, "page"> page) {
                    return mach::ok(
                        std::to_string(id.value()) + ":" +
                        std::string(context.request.routeParam("id")) + ":" +
                        std::to_string(page.value()));
                });

            m_app.mapGet(
                "/explicit/context/last/{id}",
                [](mach::Param<int, "id"> id, mach::Context& context) {
                    return mach::ok(
                        std::to_string(id.value()) + ":" +
                        std::string(context.request.routeParam("id")));
                });

            // Access operators: exercise the wrappers through actual HTTP handlers.
            m_app.mapPost("/explicit/operators/body/int", [](mach::Body<int> number) {
                return mach::ok(*number * 2);
            });

            m_app.mapPost("/explicit/operators/body/user", [](mach::Body<UserDto> user) {
                return mach::ok(user->name + ":" + std::to_string(user->age));
            });

            m_app.mapGet("/explicit/operators/param/int/{id}", [](mach::Param<int, "id"> id) {
                return mach::ok(*id);
            });

            m_app.mapGet(
                "/explicit/operators/param/string/{name}",
                [](mach::Param<std::string, "name"> name) {
                    return mach::ok(std::to_string(name->size()));
                });

            m_app.mapGet("/explicit/operators/query/int", [](mach::Query<int, "page"> page) {
                return mach::ok(*page);
            });

            m_app.mapGet(
                "/explicit/operators/query/optional",
                [](mach::Query<std::optional<int>, "page"> page) {
                    return mach::ok(std::to_string(page->value_or(1)));
                });

            m_app.mapPost("/explicit/legacy", [](UserDto user) {
                return mach::ok(user.name);
            });
        }

        void waitUntilReady() {
            BeastHttpClient client;
            for (int attempt = 0; attempt < 100; ++attempt) {
                try {
                    (void)client.get("/explicit/ready");
                    return;
                } catch (...) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
            }
            throw std::runtime_error("Explicit binding test server failed to start");
        }

        mach::App m_app;
        std::thread m_thread;
    };

    ExplicitBindingServer& server() {
        static ExplicitBindingServer instance;
        return instance;
    }

    BeastHttpClient& client() {
        static BeastHttpClient instance;
        return instance;
    }

    void ensureServerRunning() {
        (void)server();
    }

    BeastHttpClient::Headers jsonHeaders() {
        return {{"Content-Type", "application/json"}};
    }
}

TEST_CASE("Minimal API explicit body binding") {
    ensureServerRunning();

    SECTION("DTO") {
        const auto response =
            client().post("/explicit/body/user", R"({"name":"Asaf","age":18})", jsonHeaders());
        REQUIRE(response.statusCode() == 200);
        REQUIRE(response.body() == "Asaf:18");
    }

    SECTION("Integer") {
        const auto response = client().post("/explicit/body/int", "42", jsonHeaders());
        REQUIRE(response.statusCode() == 200);
        REQUIRE(response.body() == "42");
    }

    SECTION("JSON string") {
        const auto response = client().post("/explicit/body/string", R"("hello")", jsonHeaders());
        REQUIRE(response.statusCode() == 200);
        REQUIRE(response.body() == "hello");
    }

    SECTION("Malformed JSON") {
        const auto response =
            client().post("/explicit/body/user", R"({"name":"Asaf","age":18)", jsonHeaders());
        REQUIRE(response.statusCode() == 400);
    }

    SECTION("Incorrect DTO field type") {
        const auto response = client().post(
            "/explicit/body/user",
            R"({"name":"Asaf","age":"invalid"})",
            jsonHeaders());
        REQUIRE(response.statusCode() == 400);
    }

    SECTION("Unsupported Content-Type") {
        const auto response =
            client().post("/explicit/body/int", "42", {{"Content-Type", "text/plain"}});
        REQUIRE(response.statusCode() == 415);
    }

    SECTION("Missing Content-Type") {
        const auto response = client().post("/explicit/body/int", "42");
        REQUIRE(response.statusCode() == 415);
    }
}

TEST_CASE("Minimal API explicit route parameter binding") {
    ensureServerRunning();

    SECTION("Integer") {
        const auto response = client().get("/explicit/param/int/42");
        REQUIRE(response.statusCode() == 200);
        REQUIRE(response.body() == "42");
    }

    SECTION("String") {
        const auto response = client().get("/explicit/param/string/Alice");
        REQUIRE(response.statusCode() == 200);
        REQUIRE(response.body() == "Alice");
    }

    SECTION("Invalid integer") {
        const auto response = client().get("/explicit/param/int/not-a-number");
        REQUIRE(response.statusCode() == 400);
    }
}

TEST_CASE("Minimal API explicit query parameter binding") {
    ensureServerRunning();

    SECTION("Integer") {
        const auto response = client().get("/explicit/query/int?page=12");
        REQUIRE(response.statusCode() == 200);
        REQUIRE(response.body() == "12");
    }

    SECTION("String") {
        const auto response = client().get("/explicit/query/string?term=hello");
        REQUIRE(response.statusCode() == 200);
        REQUIRE(response.body() == "hello");
    }

    SECTION("Missing required parameter") {
        const auto response = client().get("/explicit/query/int");
        REQUIRE(response.statusCode() == 400);
    }

    SECTION("Invalid integer") {
        const auto response = client().get("/explicit/query/int?page=invalid");
        REQUIRE(response.statusCode() == 400);
    }

    SECTION("Missing optional parameter") {
        const auto response = client().get("/explicit/query/optional");
        REQUIRE(response.statusCode() == 200);
        REQUIRE(response.body() == "none");
    }

    SECTION("Present optional parameter") {
        const auto response = client().get("/explicit/query/optional?page=7");
        REQUIRE(response.statusCode() == 200);
        REQUIRE(response.body() == "7");
    }
}

TEST_CASE("Minimal API combines explicit binding sources") {
    ensureServerRunning();

    SECTION("Route and query") {
        const auto response = client().get("/explicit/combined/42?page=3");
        REQUIRE(response.statusCode() == 200);
        REQUIRE(response.body() == "42:3");
    }

    SECTION("Query, body and route in noncanonical order") {
        const auto response =
            client().post("/explicit/all/42?page=3", R"({"name":"Alice","age":30})", jsonHeaders());
        REQUIRE(response.statusCode() == 200);
        REQUIRE(response.body() == "42:Alice:3");
    }

    SECTION("Multiple route parameters") {
        const auto response = client().get("/explicit/params/42/7");
        REQUIRE(response.statusCode() == 200);
        REQUIRE(response.body() == "42:7");
    }

    SECTION("Multiple query parameters") {
        const auto response = client().get("/explicit/queries?page=3&sort=desc");
        REQUIRE(response.statusCode() == 200);
        REQUIRE(response.body() == "3:desc");
    }

    SECTION("Invalid query in combined binding") {
        const auto response = client().post(
            "/explicit/all/42?page=invalid",
            R"({"name":"Alice","age":30})",
            jsonHeaders());

        REQUIRE(response.statusCode() == 400);
    }
}

TEST_CASE("Minimal API explicit bindings coexist with Context references") {
    ensureServerRunning();

    SECTION("Mutable Context first") {
        const auto response = client().get("/explicit/context/first/42");
        REQUIRE(response.statusCode() == 200);
        REQUIRE(response.body() == "42:42");
    }

    SECTION("Const Context in the middle") {
        const auto response = client().get("/explicit/context/middle/42?page=3");
        REQUIRE(response.statusCode() == 200);
        REQUIRE(response.body() == "42:42:3");
    }

    SECTION("Mutable Context last") {
        const auto response = client().get("/explicit/context/last/42");
        REQUIRE(response.statusCode() == 200);
        REQUIRE(response.body() == "42:42");
    }
}

TEST_CASE("Minimal API preserves legacy implicit body binding") {
    ensureServerRunning();

    const auto response =
        client().post("/explicit/legacy", R"({"name":"Alice","age":30})", jsonHeaders());
    REQUIRE(response.statusCode() == 200);
    REQUIRE(response.body() == "Alice");
}

TEST_CASE("Minimal API binding wrapper access operators") {
    ensureServerRunning();

    SECTION("Body dereference for integer") {
        const auto response = client().post("/explicit/operators/body/int", "21", jsonHeaders());
        REQUIRE(response.statusCode() == 200);
        REQUIRE(response.body() == "42");
    }

    SECTION("Body member access for DTO") {
        const auto response = client().post(
            "/explicit/operators/body/user",
            R"({"name":"Alice","age":30})",
            jsonHeaders());
        REQUIRE(response.statusCode() == 200);
        REQUIRE(response.body() == "Alice:30");
    }

    SECTION("Param dereference for integer") {
        const auto response = client().get("/explicit/operators/param/int/42");
        REQUIRE(response.statusCode() == 200);
        REQUIRE(response.body() == "42");
    }

    SECTION("Param member access for string") {
        const auto response = client().get("/explicit/operators/param/string/Alice");
        REQUIRE(response.statusCode() == 200);
        REQUIRE(response.body() == "5");
    }

    SECTION("Query dereference for integer") {
        const auto response = client().get("/explicit/operators/query/int?page=12");
        REQUIRE(response.statusCode() == 200);
        REQUIRE(response.body() == "12");
    }

    SECTION("Query member access for present optional") {
        const auto response = client().get("/explicit/operators/query/optional?page=7");
        REQUIRE(response.statusCode() == 200);
        REQUIRE(response.body() == "7");
    }

    SECTION("Query member access for missing optional") {
        const auto response = client().get("/explicit/operators/query/optional");
        REQUIRE(response.statusCode() == 200);
        REQUIRE(response.body() == "1");
    }
}
