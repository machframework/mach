#include <catch2/catch_test_macros.hpp>

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

#include <mach/Request.hpp>

#include "support/TestRequestBuilder.hpp"

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

// Media type inspection

TEST_CASE(
    "Request media type inspection handles missing Content-Type headers",
    "[request][headers]") {
    auto context = TestRequestBuilder{}.method(mach::http::Method::Get).target("/").build();

    CHECK_FALSE(context.request.mediaType().has_value());
    CHECK_FALSE(context.request.charset().has_value());
    CHECK_FALSE(context.request.isContentType("application/json"));
    CHECK_FALSE(context.request.isJson());
    CHECK_FALSE(context.request.isMultipartFormData());
    CHECK_FALSE(context.request.isFormUrlEncoded());
}

TEST_CASE("Request media type inspection extracts media type and charset", "[request][headers]") {
    auto context = TestRequestBuilder{}
                       .method(mach::http::Method::Get)
                       .target("/")
                       .header("Content-Type", "Application/JSON; Charset=\"UTF-8\"")
                       .build(false);

    const auto mediaType = context.request.mediaType();
    const auto charset = context.request.charset();

    REQUIRE(mediaType.has_value());
    REQUIRE(charset.has_value());

    CHECK(std::string(*mediaType) == "Application/JSON");
    CHECK(std::string(*charset) == "UTF-8");
}

TEST_CASE(
    "Request media type inspection performs case-insensitive exact matching",
    "[request][headers]") {
    auto context = TestRequestBuilder{}
                       .method(mach::http::Method::Get)
                       .target("/")
                       .header("Content-Type", "Application/JSON; charset=utf-8")
                       .build(false);

    CHECK(context.request.isContentType("application/json"));
    CHECK(context.request.isContentType("APPLICATION/JSON"));
    CHECK_FALSE(context.request.isContentType("application/xml"));
    CHECK_FALSE(context.request.isContentType("application/*"));
    CHECK_FALSE(context.request.isContentType("application/json; charset=utf-8"));
    CHECK_FALSE(context.request.isContentType(""));
}

TEST_CASE("Request media type inspection recognizes JSON media types", "[request][headers]") {
    struct TestCase {
        std::string_view contentType;
        bool expected;
    };

    constexpr TestCase testCases[] = {
        {"application/json", true},
        {"Application/JSON", true},
        {"application/json; charset=utf-8", true},
        {"application/problem+json", true},
        {"application/vnd.api+JSON", true},
        {"application/+json", false},
        {"application/jsonp", false},
        {"text/json", false},
        {"text/plain", false}};

    for (const auto& testCase : testCases) {
        CAPTURE(std::string(testCase.contentType));

        auto context = TestRequestBuilder{}
                           .method(mach::http::Method::Get)
                           .target("/")
                           .header("Content-Type", testCase.contentType)
                           .build(false);

        CHECK(context.request.isJson() == testCase.expected);
    }
}

TEST_CASE("Request media type inspection recognizes form media types", "[request][headers]") {
    struct TestCase {
        std::string_view contentType;
        bool multipart;
        bool urlEncoded;
    };

    constexpr TestCase testCases[] = {
        {"multipart/form-data", true, false},
        {"multipart/form-data; boundary=abc123", true, false},
        {"Multipart/Form-Data", true, false},
        {"application/x-www-form-urlencoded", false, true},
        {"application/x-www-form-urlencoded; charset=utf-8", false, true},
        {"Application/X-WWW-Form-Urlencoded", false, true},
        {"multipart/mixed", false, false},
        {"application/json", false, false},
        {"text/plain", false, false}};

    for (const auto& testCase : testCases) {
        CAPTURE(std::string(testCase.contentType));

        auto context = TestRequestBuilder{}
                           .method(mach::http::Method::Get)
                           .target("/")
                           .header("Content-Type", testCase.contentType)
                           .build(false);

        CHECK(context.request.isMultipartFormData() == testCase.multipart);
        CHECK(context.request.isFormUrlEncoded() == testCase.urlEncoded);
    }
}

TEST_CASE(
    "Request media type inspection handles empty and malformed header values",
    "[request][headers]") {
    constexpr std::string_view invalidMediaTypes[] = {
        "",
        "application",
        "application/",
        "/json",
        "application / json",
        "application/json/extra",
        "application/*"};

    for (const auto contentType : invalidMediaTypes) {
        CAPTURE(std::string(contentType));

        auto context = TestRequestBuilder{}
                           .method(mach::http::Method::Get)
                           .target("/")
                           .header("Content-Type", contentType)
                           .build(false);

        CHECK_FALSE(context.request.mediaType().has_value());
        CHECK_FALSE(context.request.charset().has_value());
        CHECK_FALSE(context.request.isContentType("application/json"));
        CHECK_FALSE(context.request.isJson());
        CHECK_FALSE(context.request.isMultipartFormData());
        CHECK_FALSE(context.request.isFormUrlEncoded());
    }

    constexpr std::string_view invalidCharsets[] = {
        "application/json; charset=",
        "application/json; charset=\"\"",
        "application/json; charset=utf-8; charset=utf-16"};

    for (const auto contentType : invalidCharsets) {
        CAPTURE(std::string(contentType));

        auto context = TestRequestBuilder{}
                           .method(mach::http::Method::Get)
                           .target("/")
                           .header("Content-Type", contentType)
                           .build(false);

        const auto mediaType = context.request.mediaType();

        REQUIRE(mediaType.has_value());

        CHECK(std::string(*mediaType) == "application/json");
        CHECK_FALSE(context.request.charset().has_value());
        CHECK(context.request.isContentType("application/json"));
        CHECK(context.request.isJson());
    }

    auto context = TestRequestBuilder{}
                       .method(mach::http::Method::Get)
                       .target("/")
                       .header("Content-Type", "application/json")
                       .build(false);

    CHECK_FALSE(context.request.charset().has_value());
    CHECK(context.request.isJson());
}

// Content length inspection

TEST_CASE("Request content length inspection handles missing headers", "[request][headers]") {
    auto context = TestRequestBuilder{}.method(mach::http::Method::Get).target("/").build(false);

    CHECK_FALSE(context.request.contentLengthBytes().has_value());
}

TEST_CASE("Request content length inspection parses valid header values", "[request][headers]") {
    struct TestCase {
        std::string_view value;
        std::uint64_t expected;
    };

    constexpr TestCase testCases[] =
        {{"0", 0}, {"1024", 1024}, {"001024", 1024}, {"4294967296", 4294967296ULL}};

    for (const auto& testCase : testCases) {
        CAPTURE(std::string(testCase.value));

        auto context = TestRequestBuilder{}
                           .method(mach::http::Method::Get)
                           .target("/")
                           .header("Content-Length", testCase.value)
                           .build(false);

        const auto result = context.request.contentLengthBytes();

        REQUIRE(result.has_value());
        CHECK(*result == testCase.expected);
    }
}

TEST_CASE("Request content length inspection rejects invalid header values", "[request][headers]") {
    constexpr std::string_view invalidValues[] =
        {"", "-1", "1024abc", "1024, 1024", "18446744073709551616"};

    for (const auto value : invalidValues) {
        CAPTURE(std::string(value));

        auto context = TestRequestBuilder{}
                           .method(mach::http::Method::Get)
                           .target("/")
                           .header("Content-Length", value)
                           .build(false);

        CHECK_FALSE(context.request.contentLengthBytes().has_value());
    }
}