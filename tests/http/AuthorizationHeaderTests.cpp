#include <catch2/catch_test_macros.hpp>

#include <string>
#include <string_view>

#include "http/AuthorizationHeader.hpp"

namespace
{
    using mach::detail::http::extractAuthorizationScheme;
    using mach::detail::http::extractBearerToken;

    struct ExtractionTestCase {
        std::string_view authorization;
        std::string_view expected;
    };
}

// Authentication scheme extraction

TEST_CASE("Authorization scheme extraction recognizes valid schemes", "[http][authorization]") {
    constexpr ExtractionTestCase testCases[] = {
        {"Bearer abc123", "Bearer"},
        {"bearer abc123", "bearer"},
        {"BEARER abc123", "BEARER"},
        {"Basic dXNlcjpwYXNz", "Basic"},
        {"Digest username=\"user\"", "Digest"},
        {"Negotiate credentials", "Negotiate"},
        {"Custom credentials", "Custom"},
        {"Custom-Scheme credentials", "Custom-Scheme"},
        {"Scheme123 credentials", "Scheme123"},
        {"A!#$%&'*+-.^_`|~ credentials", "A!#$%&'*+-.^_`|~"}};

    for (const auto& testCase : testCases) {
        CAPTURE(std::string(testCase.authorization));

        const auto result = extractAuthorizationScheme(testCase.authorization);

        REQUIRE(result.has_value());
        CHECK(std::string(*result) == std::string(testCase.expected));
    }
}

TEST_CASE("Authorization scheme extraction handles whitespace", "[http][authorization]") {
    constexpr ExtractionTestCase testCases[] = {
        {" Bearer abc123", "Bearer"},
        {"Bearer abc123 ", "Bearer"},
        {"  Bearer abc123  ", "Bearer"},
        {"\tBearer abc123\t", "Bearer"},
        {" \tBearer abc123\t ", "Bearer"},
        {"Bearer    abc123", "Bearer"},
        {"Bearer \tabc123", "Bearer"}};

    for (const auto& testCase : testCases) {
        CAPTURE(std::string(testCase.authorization));

        const auto result = extractAuthorizationScheme(testCase.authorization);

        REQUIRE(result.has_value());
        CHECK(std::string(*result) == std::string(testCase.expected));
    }
}

TEST_CASE(
    "Authorization scheme extraction does not validate credentials",
    "[http][authorization]") {
    constexpr ExtractionTestCase testCases[] = {
        {"Bearer", "Bearer"},
        {"Bearer ", "Bearer"},
        {"Bearer\t", "Bearer"},
        {"Basic", "Basic"},
        {"Custom", "Custom"},
        {"Bearer abc def", "Bearer"},
        {"Bearer abc=def", "Bearer"},
        {"Bearer =", "Bearer"},
        {"Bearer \"abc123\"", "Bearer"}};

    for (const auto& testCase : testCases) {
        CAPTURE(std::string(testCase.authorization));

        const auto result = extractAuthorizationScheme(testCase.authorization);

        REQUIRE(result.has_value());
        CHECK(std::string(*result) == std::string(testCase.expected));
    }
}

TEST_CASE(
    "Authorization scheme extraction rejects empty and malformed schemes",
    "[http][authorization]") {
    constexpr std::string_view invalidValues[] = {
        "",
        " ",
        "\t",
        " \t ",
        "Bear@er abc123",
        "Bear/er abc123",
        "Bearer: abc123",
        "Bearer=abc123",
        "\"Bearer\" abc123",
        "Bearer\tabc123",
        "Bearer\rabc123",
        "Bearer\nabc123",
        "Bearer\vabc123",
        "Bearer\fabc123",
        "Bearer, abc123",
        std::string_view{"Bear\0er abc123", 14},
        "\xFF abc123"};

    for (const auto value : invalidValues) {
        CAPTURE(std::string(value));

        CHECK_FALSE(extractAuthorizationScheme(value).has_value());
    }
}

// Bearer token extraction

TEST_CASE("Bearer token extraction recognizes valid tokens", "[http][authorization]") {
    constexpr ExtractionTestCase testCases[] = {
        {"Bearer abc123", "abc123"},
        {"Bearer ABC123", "ABC123"},
        {"Bearer a", "a"},
        {"Bearer 0", "0"},
        {"Bearer abc.def.ghi", "abc.def.ghi"},
        {"Bearer abc-._~+/123", "abc-._~+/123"},
        {"Bearer -", "-"},
        {"Bearer .", "."},
        {"Bearer _", "_"},
        {"Bearer ~", "~"},
        {"Bearer +", "+"},
        {"Bearer /", "/"},
        {"Bearer abc=", "abc="},
        {"Bearer abc==", "abc=="},
        {"Bearer abc===", "abc==="},
        {"Bearer a====", "a===="}};

    for (const auto& testCase : testCases) {
        CAPTURE(std::string(testCase.authorization));

        const auto result = extractBearerToken(testCase.authorization);

        REQUIRE(result.has_value());
        CHECK(std::string(*result) == std::string(testCase.expected));
    }
}

TEST_CASE(
    "Bearer token extraction matches the authentication scheme case-insensitively",
    "[http][authorization]") {
    constexpr ExtractionTestCase testCases[] = {
        {"Bearer AbC123", "AbC123"},
        {"bearer AbC123", "AbC123"},
        {"BEARER AbC123", "AbC123"},
        {"bEaReR AbC123", "AbC123"}};

    for (const auto& testCase : testCases) {
        CAPTURE(std::string(testCase.authorization));

        const auto result = extractBearerToken(testCase.authorization);

        REQUIRE(result.has_value());
        CHECK(std::string(*result) == std::string(testCase.expected));
    }
}

TEST_CASE("Bearer token extraction handles permitted whitespace", "[http][authorization]") {
    constexpr ExtractionTestCase testCases[] = {
        {"Bearer abc123", "abc123"},
        {"Bearer  abc123", "abc123"},
        {"Bearer     abc123", "abc123"},
        {" Bearer abc123", "abc123"},
        {"Bearer abc123 ", "abc123"},
        {"\tBearer abc123", "abc123"},
        {"Bearer abc123\t", "abc123"},
        {" \tBearer   abc123\t ", "abc123"}};

    for (const auto& testCase : testCases) {
        CAPTURE(std::string(testCase.authorization));

        const auto result = extractBearerToken(testCase.authorization);

        REQUIRE(result.has_value());
        CHECK(std::string(*result) == std::string(testCase.expected));
    }
}

TEST_CASE("Bearer token extraction rejects other authentication schemes", "[http][authorization]") {
    constexpr std::string_view invalidValues[] = {
        "Basic abc123",
        "Digest abc123",
        "Negotiate abc123",
        "Custom abc123",
        "BearerToken abc123",
        "Bearerabc123",
        "Bear abc123",
        "Bearer-Token abc123"};

    for (const auto value : invalidValues) {
        CAPTURE(std::string(value));

        CHECK_FALSE(extractBearerToken(value).has_value());
    }
}

TEST_CASE("Bearer token extraction rejects missing credentials", "[http][authorization]") {
    constexpr std::string_view invalidValues[] =
        {"", " ", "\t", " \t ", "Bearer", "Bearer ", "Bearer    ", "Bearer\t", "Bearer \t "};

    for (const auto value : invalidValues) {
        CAPTURE(std::string(value));

        CHECK_FALSE(extractBearerToken(value).has_value());
    }
}

TEST_CASE("Bearer token extraction rejects invalid whitespace", "[http][authorization]") {
    constexpr std::string_view invalidValues[] = {
        "Bearer\tabc123",
        "Bearer \tabc123",
        "Bearer\t abc123",
        "Bearer abc 123",
        "Bearer abc\t123",
        "Bearer\rabc123",
        "Bearer\nabc123",
        "Bearer\vabc123",
        "Bearer\fabc123",
        "Bearer abc123\r",
        "Bearer abc123\n",
        "\vBearer abc123",
        "\fBearer abc123"};

    for (const auto value : invalidValues) {
        CAPTURE(std::string(value));

        CHECK_FALSE(extractBearerToken(value).has_value());
    }
}

TEST_CASE("Bearer token extraction rejects invalid token characters", "[http][authorization]") {
    constexpr std::string_view invalidValues[] = {
        "Bearer abc:123",
        "Bearer abc;123",
        "Bearer abc,123",
        "Bearer abc@123",
        "Bearer abc#123",
        "Bearer abc%123",
        "Bearer abc&123",
        "Bearer abc!123",
        "Bearer abc*123",
        "Bearer abc?123",
        "Bearer abc\\123",
        "Bearer abc(123)",
        "Bearer \"abc123\"",
        "Bearer 'abc123'",
        "Bearer abc\xFF",
        std::string_view{"Bearer abc\0def", 14}};

    for (const auto value : invalidValues) {
        CAPTURE(std::string(value));

        CHECK_FALSE(extractBearerToken(value).has_value());
    }
}

TEST_CASE("Bearer token extraction rejects invalid padding", "[http][authorization]") {
    constexpr std::string_view invalidValues[] = {
        "Bearer =",
        "Bearer ==",
        "Bearer ===",
        "Bearer =abc",
        "Bearer ==abc",
        "Bearer abc=def",
        "Bearer abc==def",
        "Bearer abc=def=",
        "Bearer abc= ="};

    for (const auto value : invalidValues) {
        CAPTURE(std::string(value));

        CHECK_FALSE(extractBearerToken(value).has_value());
    }
}

TEST_CASE(
    "Authorization extraction returns views into the original value",
    "[http][authorization]") {
    const std::string authorization = " \tBearer   AbC123== \t";

    const auto scheme = extractAuthorizationScheme(authorization);
    const auto token = extractBearerToken(authorization);

    REQUIRE(scheme.has_value());
    REQUIRE(token.has_value());

    CHECK(scheme->data() == authorization.data() + 2);
    CHECK(token->data() == authorization.data() + 11);

    CHECK(std::string(*scheme) == "Bearer");
    CHECK(std::string(*token) == "AbC123==");
}
