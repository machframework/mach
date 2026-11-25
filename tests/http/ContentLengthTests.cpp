#include <catch2/catch_test_macros.hpp>

#include "http/ContentLength.hpp"

#include <cstdint>
#include <limits>
#include <string>
#include <string_view>

using mach::detail::http::parseContentLength;

TEST_CASE("Content-Length parser accepts valid decimal values", "[http][content-length]") {
    struct TestCase {
        std::string_view value;
        std::uint64_t expected;
    };

    const TestCase testCases[] = {
        {"0", 0},
        {"1", 1},
        {"42", 42},
        {"1024", 1024},
        {"4294967296", 4294967296ULL},
        {"18446744073709551615", std::numeric_limits<std::uint64_t>::max()}};

    for (const auto& testCase : testCases) {
        CAPTURE(std::string(testCase.value));

        const auto result = parseContentLength(testCase.value);

        REQUIRE(result.has_value());
        CHECK(*result == testCase.expected);
    }
}

TEST_CASE("Content-Length parser accepts leading zeros", "[http][content-length]") {
    struct TestCase {
        std::string_view value;
        std::uint64_t expected;
    };

    const TestCase testCases[] = {
        {"00", 0},
        {"000000", 0},
        {"001", 1},
        {"001024", 1024},
        {"00018446744073709551615", std::numeric_limits<std::uint64_t>::max()}};

    for (const auto& testCase : testCases) {
        CAPTURE(std::string(testCase.value));

        const auto result = parseContentLength(testCase.value);

        REQUIRE(result.has_value());
        CHECK(*result == testCase.expected);
    }
}

TEST_CASE(
    "Content-Length parser accepts surrounding optional whitespace",
    "[http][content-length]") {
    const std::string_view values[] =
        {" 1024", "1024 ", " 1024 ", "\t1024", "1024\t", "\t1024\t", " \t1024\t "};

    for (const auto value : values) {
        CAPTURE(std::string(value));

        const auto result = parseContentLength(value);

        REQUIRE(result.has_value());
        CHECK(*result == 1024);
    }
}

TEST_CASE("Content-Length parser rejects empty values", "[http][content-length]") {
    const std::string_view values[] = {"", " ", "\t", " \t "};

    for (const auto value : values) {
        CAPTURE(std::string(value));

        CHECK_FALSE(parseContentLength(value).has_value());
    }
}

TEST_CASE("Content-Length parser rejects signed values", "[http][content-length]") {
    const std::string_view values[] =
        {"+0", "-0", "+1", "-1", "+1024", "-1024", " +1024 ", "\t-1024\t"};

    for (const auto value : values) {
        CAPTURE(std::string(value));

        CHECK_FALSE(parseContentLength(value).has_value());
    }
}

TEST_CASE("Content-Length parser rejects invalid decimal values", "[http][content-length]") {
    const std::string_view values[] = {
        "abc",
        "1024abc",
        "abc1024",
        "10.5",
        "1e3",
        "0x400",
        "0b100",
        "1_024",
        "1 024",
        "1\t024",
        "\"1024\"",
        std::string_view{"1024\0", 5}};

    for (const auto value : values) {
        CAPTURE(std::string(value));

        CHECK_FALSE(parseContentLength(value).has_value());
    }
}

TEST_CASE("Content-Length parser rejects non-HTTP whitespace", "[http][content-length]") {
    const std::string_view values[] = {
        "\r1024",
        "1024\r",
        "\n1024",
        "1024\n",
        "\v1024",
        "1024\v",
        "\f1024",
        "1024\f",
        "\r\n1024\r\n"};

    for (const auto value : values) {
        CAPTURE(std::string(value));

        CHECK_FALSE(parseContentLength(value).has_value());
    }
}

TEST_CASE("Content-Length parser rejects comma-separated values", "[http][content-length]") {
    const std::string_view values[] =
        {"1024,1024", "1024, 1024", "1024,2048", "1024, 2048", "1024,", ",1024", ","};

    for (const auto value : values) {
        CAPTURE(std::string(value));

        CHECK_FALSE(parseContentLength(value).has_value());
    }
}

TEST_CASE("Content-Length parser rejects integer overflow", "[http][content-length]") {
    const std::string_view values[] = {
        "18446744073709551616",
        "184467440737095516150",
        "999999999999999999999999999999",
        "00018446744073709551616"};

    for (const auto value : values) {
        CAPTURE(std::string(value));

        CHECK_FALSE(parseContentLength(value).has_value());
    }
}
