#include <catch2/catch_test_macros.hpp>

#include <mach/Response.hpp>

#include <chrono>
#include <initializer_list>
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

// Typed header setters

TEST_CASE("Response date setters format and overwrite header values", "[response][headers]") {
    using namespace std::chrono;

    const system_clock::time_point value =
        sys_days{year{2026} / November / 25} + hours{13} + minutes{5} + seconds{9};
    constexpr std::string_view expected = "Wed, 25 Nov 2026 13:05:09 GMT";

    struct DateTestCase {
        std::string_view name;
        void (mach::Response::*setter)(system_clock::time_point);
    };

    constexpr DateTestCase testCases[] = {
        {"Last-Modified", &mach::Response::setLastModified},
        {"Expires", &mach::Response::setExpires},
        {"Retry-After", &mach::Response::setRetryAfter},
        {"Date", &mach::Response::setDate},
    };

    for (const auto& testCase : testCases) {
        CAPTURE(std::string(testCase.name));

        auto context =
            TestRequestBuilder{}.method(mach::http::Method::Get).target("/").build(false);

        (context.response.*testCase.setter)(value);

        auto result = context.response.header(testCase.name);
        REQUIRE(result.has_value());
        CHECK(std::string(*result) == std::string(expected));

        context.response.setHeader(testCase.name, "initial-value");
        (context.response.*testCase.setter)(value);

        result = context.response.header(testCase.name);
        REQUIRE(result.has_value());
        CHECK(std::string(*result) == std::string(expected));
    }
}

TEST_CASE("Response duration setters format non-negative durations", "[response][headers]") {
    using namespace std::chrono;

    struct DurationTestCase {
        std::string_view name;
        void (mach::Response::*setter)(seconds);
    };

    constexpr DurationTestCase testCases[] = {
        {"Age", &mach::Response::setAge},
        {"Retry-After", &mach::Response::setRetryAfter},
    };

    for (const auto& testCase : testCases) {
        CAPTURE(std::string(testCase.name));

        auto context =
            TestRequestBuilder{}.method(mach::http::Method::Get).target("/").build(false);

        for (const auto value : {seconds{120}, seconds{0}, seconds::max()}) {
            CAPTURE(value.count());

            (context.response.*testCase.setter)(value);

            const auto result = context.response.header(testCase.name);
            REQUIRE(result.has_value());
            CHECK(std::string(*result) == std::to_string(value.count()));
        }
    }
}

TEST_CASE("Response duration setters reject negative durations", "[response][headers]") {
    using namespace std::chrono;

    struct DurationTestCase {
        std::string_view name;
        void (mach::Response::*setter)(seconds);
    };

    constexpr DurationTestCase testCases[] = {
        {"Age", &mach::Response::setAge},
        {"Retry-After", &mach::Response::setRetryAfter},
    };

    for (const auto& testCase : testCases) {
        CAPTURE(std::string(testCase.name));

        auto context =
            TestRequestBuilder{}.method(mach::http::Method::Get).target("/").build(false);

        context.response.setHeader(testCase.name, "120");

        CHECK_THROWS_AS((context.response.*testCase.setter)(seconds{-1}), std::invalid_argument);

        const auto result = context.response.header(testCase.name);
        REQUIRE(result.has_value());
        CHECK(std::string(*result) == "120");
    }
}

TEST_CASE("Response Allow setter formats HTTP method lists", "[response][headers]") {
    using mach::http::Method;

    auto context = TestRequestBuilder{}.method(Method::Get).target("/").build(false);

    context.response.setAllow(
        {Method::Get,
         Method::Post,
         Method::Put,
         Method::Patch,
         Method::Delete,
         Method::Head,
         Method::Options});

    auto result = context.response.header("Allow");
    REQUIRE(result.has_value());
    CHECK(std::string(*result) == "GET, POST, PUT, PATCH, DELETE, HEAD, OPTIONS");

    context.response.setAllow({Method::Get});

    result = context.response.header("Allow");
    REQUIRE(result.has_value());
    CHECK(std::string(*result) == "GET");

    context.response.setAllow({Method::Post, Method::Get, Method::Post});

    result = context.response.header("Allow");
    REQUIRE(result.has_value());
    CHECK(std::string(*result) == "POST, GET, POST");

    context.response.setAllow(std::initializer_list<Method>{});

    result = context.response.header("Allow");
    REQUIRE(result.has_value());
    CHECK(result->empty());
}

TEST_CASE("Response Allow setter rejects invalid HTTP methods", "[response][headers]") {
    using mach::http::Method;

    auto context = TestRequestBuilder{}.method(Method::Get).target("/").build(false);

    context.response.setAllow("GET");

    CHECK_THROWS_AS(
        context.response.setAllow({Method::Post, static_cast<Method>(255)}),
        std::invalid_argument);

    const auto result = context.response.header("Allow");
    REQUIRE(result.has_value());
    CHECK(std::string(*result) == "GET");
}

// Media type inspection

TEST_CASE(
    "Response media type inspection handles missing Content-Type headers",
    "[response][headers]") {
    auto context = TestRequestBuilder{}.method(mach::http::Method::Get).target("/").build(false);

    CHECK_FALSE(context.response.mediaType().has_value());
    CHECK_FALSE(context.response.charset().has_value());
    CHECK_FALSE(context.response.isContentType("application/json"));
    CHECK_FALSE(context.response.isJson());
    CHECK_FALSE(context.response.isMultipartFormData());
    CHECK_FALSE(context.response.isFormUrlEncoded());
}

TEST_CASE("Response media type inspection extracts media type and charset", "[response][headers]") {
    auto context = TestRequestBuilder{}.method(mach::http::Method::Get).target("/").build(false);
    context.response.setContentType("Application/JSON; Charset=\"UTF-8\"");

    const auto mediaType = context.response.mediaType();
    const auto charset = context.response.charset();

    REQUIRE(mediaType.has_value());
    REQUIRE(charset.has_value());
    CHECK(std::string(*mediaType) == "Application/JSON");
    CHECK(std::string(*charset) == "UTF-8");
}

TEST_CASE(
    "Response media type inspection performs case-insensitive exact matching",
    "[response][headers]") {
    auto context = TestRequestBuilder{}.method(mach::http::Method::Get).target("/").build(false);
    context.response.setContentType("Application/JSON; charset=utf-8");

    CHECK(context.response.isContentType("application/json"));
    CHECK(context.response.isContentType("APPLICATION/JSON"));
    CHECK_FALSE(context.response.isContentType("application/xml"));
    CHECK_FALSE(context.response.isContentType("application/*"));
    CHECK_FALSE(context.response.isContentType("application/json; charset=utf-8"));
    CHECK_FALSE(context.response.isContentType(""));
}

TEST_CASE("Response media type inspection recognizes JSON media types", "[response][headers]") {
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

        auto context =
            TestRequestBuilder{}.method(mach::http::Method::Get).target("/").build(false);
        context.response.setContentType(testCase.contentType);

        CHECK(context.response.isJson() == testCase.expected);
    }
}

TEST_CASE("Response media type inspection recognizes form media types", "[response][headers]") {
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

        auto context =
            TestRequestBuilder{}.method(mach::http::Method::Get).target("/").build(false);
        context.response.setContentType(testCase.contentType);

        CHECK(context.response.isMultipartFormData() == testCase.multipart);
        CHECK(context.response.isFormUrlEncoded() == testCase.urlEncoded);
    }
}

TEST_CASE(
    "Response media type inspection handles empty and malformed header values",
    "[response][headers]") {
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

        auto context =
            TestRequestBuilder{}.method(mach::http::Method::Get).target("/").build(false);
        context.response.setContentType(contentType);

        CHECK_FALSE(context.response.mediaType().has_value());
        CHECK_FALSE(context.response.charset().has_value());
        CHECK_FALSE(context.response.isContentType("application/json"));
        CHECK_FALSE(context.response.isJson());
        CHECK_FALSE(context.response.isMultipartFormData());
        CHECK_FALSE(context.response.isFormUrlEncoded());
    }

    constexpr std::string_view invalidCharsets[] = {
        "application/json; charset=",
        "application/json; charset=\"\"",
        "application/json; charset=utf-8; charset=utf-16"};

    for (const auto contentType : invalidCharsets) {
        CAPTURE(std::string(contentType));

        auto context =
            TestRequestBuilder{}.method(mach::http::Method::Get).target("/").build(false);
        context.response.setContentType(contentType);

        const auto mediaType = context.response.mediaType();
        REQUIRE(mediaType.has_value());
        CHECK(std::string(*mediaType) == "application/json");
        CHECK_FALSE(context.response.charset().has_value());
        CHECK(context.response.isContentType("application/json"));
        CHECK(context.response.isJson());
    }

    auto context = TestRequestBuilder{}.method(mach::http::Method::Get).target("/").build(false);
    context.response.setContentType("application/json");

    CHECK_FALSE(context.response.charset().has_value());
    CHECK(context.response.isJson());
}

TEST_CASE(
    "Response media type inspection reflects Content-Type replacement",
    "[response][headers]") {
    auto context = TestRequestBuilder{}.method(mach::http::Method::Get).target("/").build(false);

    context.response.setContentType("application/json; charset=utf-8");
    {
        const auto mediaType = context.response.mediaType();
        const auto charset = context.response.charset();
        REQUIRE(mediaType.has_value());
        REQUIRE(charset.has_value());
        CHECK(std::string(*mediaType) == "application/json");
        CHECK(std::string(*charset) == "utf-8");
    }
    CHECK(context.response.isJson());

    context.response.setContentType("multipart/form-data; boundary=abc123");
    {
        const auto mediaType = context.response.mediaType();
        REQUIRE(mediaType.has_value());
        CHECK(std::string(*mediaType) == "multipart/form-data");
    }
    CHECK_FALSE(context.response.charset().has_value());
    CHECK_FALSE(context.response.isContentType("application/json"));
    CHECK_FALSE(context.response.isJson());
    CHECK(context.response.isMultipartFormData());
    CHECK_FALSE(context.response.isFormUrlEncoded());

    context.response.setHeader("content-type", "application/x-www-form-urlencoded; charset=UTF-8");
    {
        const auto mediaType = context.response.mediaType();
        const auto charset = context.response.charset();
        REQUIRE(mediaType.has_value());
        REQUIRE(charset.has_value());
        CHECK(std::string(*mediaType) == "application/x-www-form-urlencoded");
        CHECK(std::string(*charset) == "UTF-8");
    }
    CHECK(context.response.isContentType("application/x-www-form-urlencoded"));
    CHECK_FALSE(context.response.isJson());
    CHECK_FALSE(context.response.isMultipartFormData());
    CHECK(context.response.isFormUrlEncoded());

    context.response.setContentType("");
    CHECK_FALSE(context.response.mediaType().has_value());
    CHECK_FALSE(context.response.charset().has_value());
    CHECK_FALSE(context.response.isContentType("application/x-www-form-urlencoded"));
    CHECK_FALSE(context.response.isJson());
    CHECK_FALSE(context.response.isMultipartFormData());
    CHECK_FALSE(context.response.isFormUrlEncoded());
}

// Content length inspection

TEST_CASE("Response content length inspection handles missing headers", "[response][headers]") {
    auto context = TestRequestBuilder{}.method(mach::http::Method::Get).target("/").build(false);

    CHECK_FALSE(context.response.contentLengthBytes().has_value());
}

TEST_CASE(
    "Response content length inspection does not fall back to body size",
    "[response][headers]") {
    auto context = TestRequestBuilder{}.method(mach::http::Method::Get).target("/").build(false);

    context.response.body("Hello, world!");

    REQUIRE_FALSE(context.response.body().empty());
    CHECK_FALSE(context.response.contentLengthBytes().has_value());
}
