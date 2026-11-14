#include <climits>
#include <string>
#include <utility>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include <mach/binding/Body.hpp>
#include <mach/Context.hpp>
#include <mach/exceptions/HttpException.hpp>
#include <mach/detail/binding/RequestBinder.hpp>
#include <mach/detail/exceptions/RequestBindingException.hpp>
#include <mach/http/Method.hpp>
#include <mach/http/StatusCode.hpp>
#include <mach/Json.hpp>

#include "support/TestRequestBuilder.hpp"

struct CreateUserRequest {
    std::string name;
    int age;
};

MACH_DEFINE_JSON(CreateUserRequest, name, age)

namespace
{

    using mach::detail::binding::RequestBinder;
    using mach::detail::binding::RequestBindingException;
    using mach::http::StatusCode;
    using testing::http::TestRequestBuilder;

    mach::Context makeJsonRequest(std::string body) {
        return TestRequestBuilder{}
            .method(mach::http::Method::Post)
            .target("/users")
            .header("Content-Type", "application/json")
            .body(std::move(body))
            .build();
    }

    template <typename Func>
    void requireHttpError(Func&& func, StatusCode expectedStatus) {
        bool caught = false;

        try {
            std::forward<Func>(func)();
        } catch (const mach::HttpException& ex) {
            caught = true;
            REQUIRE(ex.status() == expectedStatus);
        }

        REQUIRE(caught);
    }

} // namespace

TEST_CASE("RequestBinder binds an integer JSON body", "[binding][request-binder]") {
    auto context = makeJsonRequest("42");
    RequestBinder binder;

    auto result = binder.bind<mach::Body<int>>(context.request);

    REQUIRE(result.value() == 42);
}

TEST_CASE("RequestBinder binds a string JSON body", "[binding][request-binder]") {
    auto context = makeJsonRequest(R"("hello")");
    RequestBinder binder;

    auto result = binder.bind<mach::Body<std::string>>(context.request);

    REQUIRE(result.value() == "hello");
}

TEST_CASE("RequestBinder binds a boolean JSON body", "[binding][request-binder]") {
    auto context = makeJsonRequest("true");
    RequestBinder binder;

    auto result = binder.bind<mach::Body<bool>>(context.request);

    REQUIRE(result.value());
}

TEST_CASE("RequestBinder binds structured JSON objects", "[binding][request-binder]") {
    auto context = makeJsonRequest(R"({"name":"Alice","age":25})");

    RequestBinder binder;

    auto result = binder.bind<mach::Body<CreateUserRequest>>(context.request);

    REQUIRE(result.value().name == "Alice");
    REQUIRE(result.value().age == 25);
}

TEST_CASE("RequestBinder rejects malformed JSON", "[binding][request-binder]") {
    auto context = makeJsonRequest(R"({"name":)");
    RequestBinder binder;

    REQUIRE_THROWS_AS(binder.bind<mach::Body<int>>(context.request), RequestBindingException);
}

TEST_CASE("RequestBinder rejects incompatible JSON types", "[binding][request-binder]") {
    auto context = makeJsonRequest(R"("not an integer")");
    RequestBinder binder;

    REQUIRE_THROWS_AS(binder.bind<mach::Body<int>>(context.request), RequestBindingException);
}

TEST_CASE("RequestBinder rejects unsupported Content-Type", "[binding][request-binder]") {
    auto context = TestRequestBuilder{}
                       .method(mach::http::Method::Post)
                       .target("/users")
                       .header("Content-Type", "text/plain")
                       .body("42")
                       .build();

    RequestBinder binder;

    requireHttpError(
        [&] {
            (void)binder.bind<mach::Body<int>>(context.request);
        },
        StatusCode::UnsupportedMediaType);
}

TEST_CASE("RequestBinder rejects missing Content-Type", "[binding][request-binder]") {
    auto context =
        TestRequestBuilder{}.method(mach::http::Method::Post).target("/users").body("42").build();

    RequestBinder binder;

    requireHttpError(
        [&] {
            (void)binder.bind<mach::Body<int>>(context.request);
        },
        StatusCode::UnsupportedMediaType);
}

TEST_CASE("RequestBinder rejects empty JSON bodies", "[binding][request-binder]") {
    auto context = makeJsonRequest("");
    RequestBinder binder;

    REQUIRE_THROWS_AS(binder.bind<mach::Body<int>>(context.request), RequestBindingException);
}

TEST_CASE("RequestBinder rejects JSON null for non-nullable types", "[binding][request-binder]") {
    auto context = makeJsonRequest("null");
    RequestBinder binder;

    REQUIRE_THROWS_AS(binder.bind<mach::Body<int>>(context.request), RequestBindingException);
}

TEST_CASE("RequestBinder binds JSON arrays", "[binding][request-binder]") {
    auto context = makeJsonRequest("[1, 2, 3]");
    RequestBinder binder;

    auto result = binder.bind<mach::Body<std::vector<int>>>(context.request);

    REQUIRE(result.value() == std::vector<int>{1, 2, 3});
}

TEST_CASE("RequestBinder accepts JSON Content-Type with charset", "[binding][request-binder]") {
    auto context = TestRequestBuilder{}
                       .method(mach::http::Method::Post)
                       .target("/users")
                       .header("Content-Type", "application/json; charset=utf-8")
                       .body("42")
                       .build();

    RequestBinder binder;

    auto result = binder.bind<mach::Body<int>>(context.request);

    REQUIRE(result.value() == 42);
}

TEST_CASE("RequestBinder rejects invalid DTO field types", "[binding][request-binder]") {
    auto context = makeJsonRequest(R"({"name":"Alice","age":"invalid"})");

    RequestBinder binder;

    REQUIRE_THROWS_AS(
        binder.bind<mach::Body<CreateUserRequest>>(context.request),
        RequestBindingException);
}

TEST_CASE("RequestBinder binds integer boundary values", "[binding][request-binder]") {
    RequestBinder binder;

    SECTION("Minimum integer") {
        auto context = makeJsonRequest(std::to_string(INT_MIN));

        auto result = binder.bind<mach::Body<int>>(context.request);

        REQUIRE(result.value() == INT_MIN);
    }

    SECTION("Maximum integer") {
        auto context = makeJsonRequest(std::to_string(INT_MAX));

        auto result = binder.bind<mach::Body<int>>(context.request);

        REQUIRE(result.value() == INT_MAX);
    }
}
