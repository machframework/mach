#include <catch2/catch_test_macros.hpp>

#include <mach/Request.hpp>

#include "support/TestRequestBuilder.hpp"

#include <optional>
#include <string>
#include <string_view>

namespace
{
    using testing::http::TestRequestBuilder;

    struct HeaderTestCase {
        std::string_view name;
        std::string_view lowercaseName;
        std::string_view value;
        std::optional<std::string_view> (mach::Request::*getter)() const;
    };

    constexpr HeaderTestCase headerTestCases[] = {
        // Content and representation

        {"Content-Type", "content-type", "application/json", &mach::Request::contentType},
        {"Content-Length", "content-length", "1024", &mach::Request::contentLength},
        {"Content-Encoding", "content-encoding", "gzip", &mach::Request::contentEncoding},
        {"Content-Language", "content-language", "en-US", &mach::Request::contentLanguage},

        // Content negotiation

        {"Accept", "accept", "application/xml", &mach::Request::accept},
        {"Accept-Encoding", "accept-encoding", "gzip, deflate", &mach::Request::acceptEncoding},
        {"Accept-Language", "accept-language", "en-US, en;q=0.9", &mach::Request::acceptLanguage},

        // Authentication

        {"Authorization", "authorization", "Bearer abc123", &mach::Request::authorization},

        // Client and origin information

        {"Host", "host", "example.com", &mach::Request::host},
        {"User-Agent", "user-agent", "MachTestClient/1.0", &mach::Request::userAgent},
        {"Origin", "origin", "https://example.com", &mach::Request::origin},
        {"Referer", "referer", "https://example.com/page", &mach::Request::referer},

        // Caching and conditional requests

        {"Cache-Control", "cache-control", "no-cache", &mach::Request::cacheControl},
        {"If-Match", "if-match", "\"abc123\"", &mach::Request::ifMatch},
        {"If-None-Match", "if-none-match", "\"def456\"", &mach::Request::ifNoneMatch},
        {"If-Modified-Since",
         "if-modified-since",
         "Sun, 06 Nov 1994 08:49:37 GMT",
         &mach::Request::ifModifiedSince},
        {"If-Unmodified-Since",
         "if-unmodified-since",
         "Mon, 07 Nov 1994 08:49:37 GMT",
         &mach::Request::ifUnmodifiedSince},

        // Partial content

        {"Range", "range", "bytes=0-1023", &mach::Request::range},
        {"If-Range", "if-range", "\"ghi789\"", &mach::Request::ifRange},

        // Proxy and forwarding

        {"Forwarded", "forwarded", "for=192.0.2.1;proto=https", &mach::Request::forwarded},
        {"X-Forwarded-For", "x-forwarded-for", "192.0.2.2", &mach::Request::xForwardedFor},
        {"X-Forwarded-Host",
         "x-forwarded-host",
         "proxy.example.com",
         &mach::Request::xForwardedHost},
        {"X-Forwarded-Proto", "x-forwarded-proto", "https", &mach::Request::xForwardedProto}};

} // namespace


// Raw header getters

TEST_CASE("Request header convenience getters return stored values", "[request][headers]") {
    for (const auto& testCase : headerTestCases) {
        CAPTURE(std::string(testCase.name));

        auto context = TestRequestBuilder{}
                           .method(mach::http::Method::Get)
                           .target("/")
                           .header(testCase.name, testCase.value)
                           .build(false);

        const auto result = (context.request.*testCase.getter)();

        REQUIRE(result.has_value());
        CHECK(std::string(*result) == std::string(testCase.value));
    }
}

TEST_CASE(
    "Request header convenience getters return nullopt for missing headers",
    "[request][headers]") {
    auto context = TestRequestBuilder{}.method(mach::http::Method::Get).target("/").build();

    for (const auto& testCase : headerTestCases) {
        CAPTURE(std::string(testCase.name));

        const auto result = (context.request.*testCase.getter)();

        CHECK_FALSE(result.has_value());
    }
}

TEST_CASE("Request header convenience getters preserve empty header values", "[request][headers]") {
    for (const auto& testCase : headerTestCases) {
        CAPTURE(std::string(testCase.name));

        auto context = TestRequestBuilder{}
                           .method(mach::http::Method::Get)
                           .target("/")
                           .header(testCase.name, "")
                           .build(false);

        const auto result = (context.request.*testCase.getter)();

        REQUIRE(result.has_value());
        CHECK(result->empty());
    }
}

TEST_CASE(
    "Request header convenience getters perform case-insensitive lookup",
    "[request][headers]") {
    for (const auto& testCase : headerTestCases) {
        CAPTURE(std::string(testCase.name), std::string(testCase.lowercaseName));

        auto context = TestRequestBuilder{}
                           .method(mach::http::Method::Get)
                           .target("/")
                           .header(testCase.lowercaseName, testCase.value)
                           .build(false);

        const auto result = (context.request.*testCase.getter)();

        REQUIRE(result.has_value());
        CHECK(std::string(*result) == std::string(testCase.value));
    }
}
