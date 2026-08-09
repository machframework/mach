#include "Testing.hpp"

#include <boost/beast/http.hpp>

#include "adapter/inbound/BeastRequestAdapter.hpp"

namespace http = boost::beast::http;

static http::request<http::string_body> makeRequest(
    http::verb method = http::verb::get,
    std::string target = "/",
    std::string body = "");

#include <boost/beast/http.hpp>

#include <string>
#include <string_view>

#include <mach/http/Method.hpp>
#include <mach/http/StatusCode.hpp>
#include <mach/http/Version.hpp>

namespace http = boost::beast::http;

static void validRequestAdaptsCorrectly() {
    constexpr std::string_view testName = "Valid request adapts correctly";

    auto raw = makeRequest(http::verb::post, "/users/123", "hello");
    raw.set(http::field::content_type, "application/json");
    raw.set("X-Custom-Header", "SomeValue");

    bool rejected = false;
    mach::detail::http::adapter::BeastRequestAdapter adapter;
    auto context = adapter.adapt(std::move(raw), rejected);

    if (rejected) {
        testing::fail(testName, "Valid request was rejected");
        return;
    }

    if (context.request.method() != mach::http::Method::Post) {
        testing::fail(testName, "Method was not adapted correctly");
        return;
    }

    if (context.request.version() != mach::http::Version::Http11) {
        testing::fail(testName, "Version was not adapted correctly");
        return;
    }

    if (context.request.target() != "/users/123") {
        testing::fail(testName, "Target was not adapted correctly");
        return;
    }

    if (context.request.body() != "hello") {
        testing::fail(testName, "Body was not adapted correctly");
        return;
    }

    testing::success(testName);
}

static void headerNamesAreLowercased() {
    constexpr std::string_view testName = "Header names are lowercased";

    auto raw = makeRequest();
    raw.set("X-Custom-Header", "abc");
    raw.set(http::field::content_type, "text/plain");

    bool rejected = false;
    mach::detail::http::adapter::BeastRequestAdapter adapter;

    auto context = adapter.adapt(std::move(raw), rejected);

    if (rejected) {
        testing::fail(testName, "Request was rejected");
        return;
    }

    if (!context.request.header("x-custom-header").has_value()) {
        testing::fail(testName, "Lowercase custom header was not found");
        return;
    }

    if (context.request.header("x-custom-header").value() != "abc") {
        testing::fail(testName, "Custom header value was not preserved");
        return;
    }

    if (!context.request.header("content-type").has_value()) {
        testing::fail(testName, "Lowercase Content-Type was not found");
        return;
    }

    testing::success(testName);
}

static void headerValuesArePreservedExactly() {
    constexpr std::string_view testName = "Header values are preserved exactly";

    auto raw = makeRequest();
    raw.set(http::field::authorization, "Bearer AbC123_XyZ");
    raw.set("X-User", "Asaf");

    bool rejected = false;
    mach::detail::http::adapter::BeastRequestAdapter adapter;

    auto context = adapter.adapt(std::move(raw), rejected);

    if (rejected) {
        testing::fail(testName, "Request was rejected");
        return;
    }

    if (context.request.header("authorization").value_or("") != "Bearer AbC123_XyZ") {
        testing::fail(testName, "Authorization value casing was not preserved");
        return;
    }

    if (context.request.header("x-user").value_or("") != "Asaf") {
        testing::fail(testName, "Custom header value casing was not preserved");
        return;
    }

    testing::success(testName);
}

static void duplicateHeaderLastValueWins() {
    constexpr std::string_view testName = "Duplicate header last value wins";

    auto raw = makeRequest();
    raw.insert("X-Test", "first");
    raw.insert("X-Test", "second");

    bool rejected = false;
    mach::detail::http::adapter::BeastRequestAdapter adapter;

    auto context = adapter.adapt(std::move(raw), rejected);

    if (rejected) {
        testing::fail(testName, "Request was rejected");
        return;
    }

    if (context.request.header("x-test").value_or("") != "second") {
        testing::fail(testName, "Duplicate header did not keep the last value");
        return;
    }

    testing::success(testName);
}

static void unsupportedMethodIsRejected() {
    constexpr std::string_view testName = "Unsupported method is rejected";

    auto raw = makeRequest(http::verb::trace, "/");

    bool rejected = false;
    mach::detail::http::adapter::BeastRequestAdapter adapter;

    auto context = adapter.adapt(std::move(raw), rejected);

    if (!rejected) {
        testing::fail(testName, "Unsupported method was not rejected");
        return;
    }

    if (context.response.status() != mach::http::StatusCode::NotImplemented) {
        testing::fail(testName, "Unsupported method did not produce 501");
        return;
    }

    if (context.response.body() != "Unsupported HTTP method.") {
        testing::fail(testName, "Wrong rejection body for unsupported method");
        return;
    }

    testing::success(testName);
}

static void unsupportedVersionIsRejected() {
    constexpr std::string_view testName = "Unsupported HTTP version is rejected";

    auto raw = makeRequest();
    raw.version(12);

    bool rejected = false;
    mach::detail::http::adapter::BeastRequestAdapter adapter;

    auto context = adapter.adapt(std::move(raw), rejected);

    if (!rejected) {
        testing::fail(testName, "Unsupported HTTP version was not rejected");
        return;
    }

    if (context.response.status() != mach::http::StatusCode::HttpVersionNotSupported) {
        testing::fail(testName, "Unsupported version did not produce 505");
        return;
    }

    if (context.response.body() != "Unsupported HTTP version.") {
        testing::fail(testName, "Wrong rejection body for unsupported version");
        return;
    }

    testing::success(testName);
}

static void http10IsAccepted() {
    constexpr std::string_view testName = "HTTP/1.0 is accepted";

    auto raw = makeRequest();
    raw.version(10);

    bool rejected = false;
    mach::detail::http::adapter::BeastRequestAdapter adapter;

    auto context = adapter.adapt(std::move(raw), rejected);

    if (rejected) {
        testing::fail(testName, "HTTP/1.0 request was rejected");
        return;
    }

    if (context.request.version() != mach::http::Version::Http10) {
        testing::fail(testName, "HTTP/1.0 was not adapted correctly");
        return;
    }

    testing::success(testName);
}

static void emptyTargetIsRejected() {
    constexpr std::string_view testName = "Empty target is rejected";

    auto raw = makeRequest(http::verb::get, "");

    bool rejected = false;
    mach::detail::http::adapter::BeastRequestAdapter adapter;

    auto context = adapter.adapt(std::move(raw), rejected);

    if (!rejected) {
        testing::fail(testName, "Empty target was not rejected");
        return;
    }

    if (context.response.status() != mach::http::StatusCode::BadRequest) {
        testing::fail(testName, "Empty target did not produce 400");
        return;
    }

    if (context.response.body() != "Request target must not be empty.") {
        testing::fail(testName, "Wrong rejection body for empty target");
        return;
    }

    testing::success(testName);
}

static void targetMustStartWithSlash() {
    constexpr std::string_view testName = "Target must start with slash";

    auto raw = makeRequest(http::verb::get, "users/123");

    bool rejected = false;
    mach::detail::http::adapter::BeastRequestAdapter adapter;

    auto context = adapter.adapt(std::move(raw), rejected);

    if (!rejected) {
        testing::fail(testName, "Target without leading slash was not rejected");
        return;
    }

    if (context.response.status() != mach::http::StatusCode::BadRequest) {
        testing::fail(testName, "Target without leading slash did not produce 400");
        return;
    }

    if (context.response.body() != "Request target must start with '/'.") {
        testing::fail(testName, "Wrong rejection body for target without leading slash");
        return;
    }

    testing::success(testName);
}

static void targetWithControlCharacterIsRejected() {
    constexpr std::string_view testName = "Target with control character is rejected";

    auto raw = makeRequest(http::verb::get, "/users/\t123");

    bool rejected = false;
    mach::detail::http::adapter::BeastRequestAdapter adapter;

    auto context = adapter.adapt(std::move(raw), rejected);

    if (!rejected) {
        testing::fail(testName, "Target with control character was not rejected");
        return;
    }

    if (context.response.status() != mach::http::StatusCode::BadRequest) {
        testing::fail(testName, "Control character target did not produce 400");
        return;
    }

    if (context.response.body() != "Request target contains control characters.") {
        testing::fail(testName, "Wrong rejection body for control character target");
        return;
    }

    testing::success(testName);
}

static void bodyIsPreservedWhenEmpty() {
    constexpr std::string_view testName = "Empty body is preserved";

    auto raw = makeRequest(http::verb::post, "/submit", "");

    bool rejected = false;
    mach::detail::http::adapter::BeastRequestAdapter adapter;

    auto context = adapter.adapt(std::move(raw), rejected);

    if (rejected) {
        testing::fail(testName, "Request with empty body was rejected");
        return;
    }

    if (!context.request.body().empty()) {
        testing::fail(testName, "Empty body did not remain empty");
        return;
    }

    testing::success(testName);
}

static void bodyIsPreservedExactly() {
    constexpr std::string_view testName = "Body is preserved exactly";

    const std::string body = R"({"name":"Asaf","role":"dev"})";

    auto raw = makeRequest(http::verb::post, "/users", body);
    raw.set(http::field::content_type, "application/json");

    bool rejected = false;
    mach::detail::http::adapter::BeastRequestAdapter adapter;

    auto context = adapter.adapt(std::move(raw), rejected);

    if (rejected) {
        testing::fail(testName, "Request with body was rejected");
        return;
    }

    if (context.request.body() != body) {
        testing::fail(testName, "Body content changed during adaptation");
        return;
    }

    testing::success(testName);
}

int main() {
    validRequestAdaptsCorrectly();
    headerNamesAreLowercased();
    headerValuesArePreservedExactly();
    duplicateHeaderLastValueWins();

    unsupportedMethodIsRejected();
    unsupportedVersionIsRejected();
    http10IsAccepted();

    emptyTargetIsRejected();
    targetMustStartWithSlash();
    targetWithControlCharacterIsRejected();

    bodyIsPreservedWhenEmpty();
    bodyIsPreservedExactly();

    testing::success("All tests completed");

    return 0;
}

http::request<http::string_body> makeRequest(
    http::verb method,
    std::string target,
    std::string body) {
    http::request<http::string_body> req;

    req.version(11);
    req.method(method);
    req.target(std::move(target));
    req.body() = std::move(body);

    req.set(http::field::host, "localhost");
    req.set(http::field::content_type, "application/json");

    req.prepare_payload();
    return req;
}
