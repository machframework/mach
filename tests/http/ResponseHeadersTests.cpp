#include <catch2/catch_test_macros.hpp>

#include <mach/Response.hpp>

#include <optional>
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
    };

    constexpr HeaderTestCase headerTestCases[] = {
        {"Content-Type", "content-type", "application/json", &mach::Response::contentType},
        {"Content-Encoding", "content-encoding", "gzip", &mach::Response::contentEncoding},
        {"Content-Language", "content-language", "en-US", &mach::Response::contentLanguage},
        {"Content-Disposition",
         "content-disposition",
         "attachment; filename=\"report.pdf\"",
         &mach::Response::contentDisposition},
        {"Content-Range", "content-range", "bytes 0-499/1000", &mach::Response::contentRange},
        {"Accept-Ranges", "accept-ranges", "bytes", &mach::Response::acceptRanges},
        {"Cache-Control", "cache-control", "public, max-age=3600", &mach::Response::cacheControl},
        {"ETag", "etag", "\"abc123\"", &mach::Response::etag},
        {"Last-Modified",
         "last-modified",
         "Tue, 24 Nov 2026 10:00:00 GMT",
         &mach::Response::lastModified},
        {"Expires", "expires", "Tue, 24 Nov 2026 11:00:00 GMT", &mach::Response::expires},
        {"Vary", "vary", "Accept-Encoding", &mach::Response::vary},
        {"Age", "age", "120", &mach::Response::age},
        {"WWW-Authenticate",
         "www-authenticate",
         "Bearer realm=\"Mach\"",
         &mach::Response::wwwAuthenticate},
        {"Location", "location", "/api/users/42", &mach::Response::location},
        {"Retry-After", "retry-after", "60", &mach::Response::retryAfter},
        {"Allow", "allow", "GET, POST, OPTIONS", &mach::Response::allow},
        {"Link", "link", "</api/users?page=2>; rel=\"next\"", &mach::Response::link},
        {"Server", "server", "Mach", &mach::Response::server},
        {"Date", "date", "Tue, 24 Nov 2026 10:00:00 GMT", &mach::Response::date},
        {"Content-Security-Policy",
         "content-security-policy",
         "default-src 'self'",
         &mach::Response::contentSecurityPolicy},
        {"Content-Security-Policy-Report-Only",
         "content-security-policy-report-only",
         "default-src 'self'; report-uri /csp-report",
         &mach::Response::contentSecurityPolicyReportOnly},
        {"Strict-Transport-Security",
         "strict-transport-security",
         "max-age=31536000; includeSubDomains",
         &mach::Response::strictTransportSecurity},
        {"X-Content-Type-Options",
         "x-content-type-options",
         "nosniff",
         &mach::Response::xContentTypeOptions},
        {"X-Frame-Options", "x-frame-options", "DENY", &mach::Response::xFrameOptions},
        {"Referrer-Policy",
         "referrer-policy",
         "strict-origin-when-cross-origin",
         &mach::Response::referrerPolicy},
        {"Permissions-Policy",
         "permissions-policy",
         "camera=(), microphone=()",
         &mach::Response::permissionsPolicy},
    };
}

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
