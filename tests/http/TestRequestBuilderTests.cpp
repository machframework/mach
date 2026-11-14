
#include <catch2/catch_test_macros.hpp>

#include <mach/http/Method.hpp>

#include "support/TestRequestBuilder.hpp"

namespace
{
    using testing::http::TestRequestBuilder;
}

TEST_CASE("TestRequestBuilder constructs a Mach request") {
    auto context = TestRequestBuilder{}
                       .method(mach::http::Method::Post)
                       .target("/users?page=2")
                       .body(R"({"name":"Asaf"})")
                       .header("Content-Type", "application/json")
                       .build();


    const auto& request = context.request;

    REQUIRE(request.method() == mach::http::Method::Post);
    REQUIRE(std::string(request.target()) == "/users?page=2");
    REQUIRE(request.body() == R"({"name":"Asaf"})");

    REQUIRE(request.header("content-type").has_value());
    REQUIRE(std::string(*request.header("content-type")) == "application/json");
}
