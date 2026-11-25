#include <catch2/catch_test_macros.hpp>

#include <mach/Response.hpp>

#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>

#include "support/TestRequestBuilder.hpp"

namespace
{
    using testing::http::TestRequestBuilder;

    struct HeaderTestCase {
        std::string_view name;
        std::string_view lowercaseName;
        std::string_view value;
        std::optional<std::string_view> (mach::Response::*getter)() const;
        void (mach::Response::*setter)(std::string_view);
    };

    constexpr HeaderTestCase headerTestCases[] = {
        {"Content-Type",
         "content-type",
         "application/json",
         &mach::Response::contentType,
         &mach::Response::setContentType},
        {"Content-Encoding",
         "content-encoding",
         "gzip",
         &mach::Response::contentEncoding,
         &mach::Response::setContentEncoding},
        {"Content-Language",
         "content-language",
         "en-US",
         &mach::Response::contentLanguage,
         &mach::Response::setContentLanguage},
        {"Content-Disposition",
         "content-disposition",
         "attachment; filename=\"report.pdf\"",
         &mach::Response::contentDisposition,
         &mach::Response::setContentDisposition},
        {"Content-Range",
         "content-range",
         "bytes 0-499/1000",
         &mach::Response::contentRange,
         &mach::Response::setContentRange},
        {"Accept-Ranges",
         "accept-ranges",
         "bytes",
         &mach::Response::acceptRanges,
         &mach::Response::setAcceptRanges},
        {"Cache-Control",
         "cache-control",
         "public, max-age=3600",
         &mach::Response::cacheControl,
         &mach::Response::setCacheControl},
        {"ETag", "etag", "\"abc123\"", &mach::Response::etag, &mach::Response::setEtag},
        {"Last-Modified",
         "last-modified",
         "Tue, 24 Nov 2026 10:00:00 GMT",
         &mach::Response::lastModified,
         &mach::Response::setLastModified},
        {"Expires",
         "expires",
         "Tue, 24 Nov 2026 11:00:00 GMT",
         &mach::Response::expires,
         &mach::Response::setExpires},
        {"Vary", "vary", "Accept-Encoding", &mach::Response::vary, &mach::Response::setVary},
        {"Age", "age", "120", &mach::Response::age, &mach::Response::setAge},
        {"WWW-Authenticate",
         "www-authenticate",
         "Bearer realm=\"Mach\"",
         &mach::Response::wwwAuthenticate,
         &mach::Response::setWwwAuthenticate},
        {"Location",
         "location",
         "/api/users/42",
         &mach::Response::location,
         &mach::Response::setLocation},
        {"Retry-After",
         "retry-after",
         "60",
         &mach::Response::retryAfter,
         &mach::Response::setRetryAfter},
        {"Allow", "allow", "GET, POST, OPTIONS", &mach::Response::allow, &mach::Response::setAllow},
        {"Link",
         "link",
         "</api/users?page=2>; rel=\"next\"",
         &mach::Response::link,
         &mach::Response::setLink},
        {"Server", "server", "Mach", &mach::Response::server, &mach::Response::setServer},
        {"Date",
         "date",
         "Tue, 24 Nov 2026 10:00:00 GMT",
         &mach::Response::date,
         &mach::Response::setDate},
        {"Content-Security-Policy",
         "content-security-policy",
         "default-src 'self'",
         &mach::Response::contentSecurityPolicy,
         &mach::Response::setContentSecurityPolicy},
        {"Content-Security-Policy-Report-Only",
         "content-security-policy-report-only",
         "default-src 'self'; report-uri /csp-report",
         &mach::Response::contentSecurityPolicyReportOnly,
         &mach::Response::setContentSecurityPolicyReportOnly},
        {"Strict-Transport-Security",
         "strict-transport-security",
         "max-age=31536000; includeSubDomains",
         &mach::Response::strictTransportSecurity,
         &mach::Response::setStrictTransportSecurity},
        {"X-Content-Type-Options",
         "x-content-type-options",
         "nosniff",
         &mach::Response::xContentTypeOptions,
         &mach::Response::setXContentTypeOptions},
        {"X-Frame-Options",
         "x-frame-options",
         "DENY",
         &mach::Response::xFrameOptions,
         &mach::Response::setXFrameOptions},
        {"Referrer-Policy",
         "referrer-policy",
         "strict-origin-when-cross-origin",
         &mach::Response::referrerPolicy,
         &mach::Response::setReferrerPolicy},
        {"Permissions-Policy",
         "permissions-policy",
         "camera=(), microphone=()",
         &mach::Response::permissionsPolicy,
         &mach::Response::setPermissionsPolicy},
    };
}

// Header getters

TEST_CASE("Response header getters return stored values", "[response][headers]") {
    for (const auto& testCase : headerTestCases) {
        CAPTURE(std::string(testCase.name));

        auto context =
            TestRequestBuilder{}.method(mach::http::Method::Get).target("/").build(false);

        context.response.setHeader(testCase.name, testCase.value);

        const auto result = (context.response.*testCase.getter)();

        REQUIRE(result.has_value());
        CHECK(std::string(*result) == std::string(testCase.value));
    }
}

TEST_CASE("Response header getters return nullopt for missing headers", "[response][headers]") {
    auto context = TestRequestBuilder{}.method(mach::http::Method::Get).target("/").build(false);

    for (const auto& testCase : headerTestCases) {
        CAPTURE(std::string(testCase.name));

        const auto result = (context.response.*testCase.getter)();

        CHECK_FALSE(result.has_value());
    }

    CHECK_FALSE(context.response.contentLength().has_value());
}

TEST_CASE("Response header getters preserve empty values", "[response][headers]") {
    for (const auto& testCase : headerTestCases) {
        CAPTURE(std::string(testCase.name));

        auto context =
            TestRequestBuilder{}.method(mach::http::Method::Get).target("/").build(false);

        context.response.setHeader(testCase.name, "");

        const auto result = (context.response.*testCase.getter)();

        REQUIRE(result.has_value());
        CHECK(result->empty());
    }
}

TEST_CASE("Response header getters are case-insensitive", "[response][headers]") {
    for (const auto& testCase : headerTestCases) {
        CAPTURE(std::string(testCase.name));

        auto context =
            TestRequestBuilder{}.method(mach::http::Method::Get).target("/").build(false);

        context.response.setHeader(testCase.lowercaseName, testCase.value);

        const auto result = (context.response.*testCase.getter)();

        REQUIRE(result.has_value());
        CHECK(std::string(*result) == std::string(testCase.value));
    }
}

TEST_CASE("Response header getters return updated values", "[response][headers]") {
    for (const auto& testCase : headerTestCases) {
        CAPTURE(std::string(testCase.name));

        auto context =
            TestRequestBuilder{}.method(mach::http::Method::Get).target("/").build(false);

        context.response.setHeader(testCase.name, "initial-value");
        context.response.setHeader(testCase.lowercaseName, testCase.value);

        const auto result = (context.response.*testCase.getter)();

        REQUIRE(result.has_value());
        CHECK(std::string(*result) == std::string(testCase.value));
    }
}

// Header setters

TEST_CASE("Response header setters store supplied values", "[response][headers]") {
    for (const auto& testCase : headerTestCases) {
        CAPTURE(std::string(testCase.name));

        auto context =
            TestRequestBuilder{}.method(mach::http::Method::Get).target("/").build(false);

        (context.response.*testCase.setter)(testCase.value);

        const auto result = context.response.header(testCase.name);

        REQUIRE(result.has_value());
        CHECK(std::string(*result) == std::string(testCase.value));
    }
}

TEST_CASE("Response header setters overwrite existing values", "[response][headers]") {
    for (const auto& testCase : headerTestCases) {
        CAPTURE(std::string(testCase.name));

        auto context =
            TestRequestBuilder{}.method(mach::http::Method::Get).target("/").build(false);

        context.response.setHeader(testCase.lowercaseName, "initial-value");

        (context.response.*testCase.setter)(testCase.value);

        const auto result = context.response.header(testCase.name);

        REQUIRE(result.has_value());
        CHECK(std::string(*result) == std::string(testCase.value));
    }
}

TEST_CASE("Response header setters accept empty values", "[response][headers]") {
    for (const auto& testCase : headerTestCases) {
        CAPTURE(std::string(testCase.name));

        auto context =
            TestRequestBuilder{}.method(mach::http::Method::Get).target("/").build(false);

        context.response.setHeader(testCase.name, testCase.value);

        (context.response.*testCase.setter)("");

        const auto result = context.response.header(testCase.name);

        REQUIRE(result.has_value());
        CHECK(result->empty());
    }
}

TEST_CASE("Response header setters reject carriage return characters", "[response][headers]") {
    for (const auto& testCase : headerTestCases) {
        CAPTURE(std::string(testCase.name));

        auto context =
            TestRequestBuilder{}.method(mach::http::Method::Get).target("/").build(false);

        context.response.setHeader(testCase.name, testCase.value);

        CHECK_THROWS_AS(
            (context.response.*testCase.setter)("invalid\rvalue"),
            std::invalid_argument);

        const auto result = context.response.header(testCase.name);

        REQUIRE(result.has_value());
        CHECK(std::string(*result) == std::string(testCase.value));
    }
}

TEST_CASE("Response header setters reject line feed characters", "[response][headers]") {
    for (const auto& testCase : headerTestCases) {
        CAPTURE(std::string(testCase.name));

        auto context =
            TestRequestBuilder{}.method(mach::http::Method::Get).target("/").build(false);

        context.response.setHeader(testCase.name, testCase.value);

        CHECK_THROWS_AS(
            (context.response.*testCase.setter)("invalid\nvalue"),
            std::invalid_argument);

        const auto result = context.response.header(testCase.name);

        REQUIRE(result.has_value());
        CHECK(std::string(*result) == std::string(testCase.value));
    }
}

TEST_CASE(
    "Response header setters store raw values without semantic validation",
    "[response][headers]") {
    for (const auto& testCase : headerTestCases) {
        CAPTURE(std::string(testCase.name));

        auto context =
            TestRequestBuilder{}.method(mach::http::Method::Get).target("/").build(false);

        constexpr std::string_view value = "arbitrary raw value";

        (context.response.*testCase.setter)(value);

        const auto result = context.response.header(testCase.name);

        REQUIRE(result.has_value());
        CHECK(std::string(*result) == std::string(value));
    }
}
