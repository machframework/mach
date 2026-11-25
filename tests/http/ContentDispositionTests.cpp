#include <catch2/catch_test_macros.hpp>

#include <stdexcept>
#include <string>
#include <string_view>

#include "http/ContentDisposition.hpp"

namespace
{
    using mach::detail::http::formatContentDisposition;

    struct FormattingTestCase {
        std::string_view filename;
        std::string_view expected;
    };
}

TEST_CASE(
    "Content disposition formatting supports both disposition types",
    "[http][content-disposition]") {
    CHECK(
        formatContentDisposition("attachment", "report.pdf") ==
        "attachment; filename=\"report.pdf\"");

    CHECK(formatContentDisposition("inline", "report.pdf") == "inline; filename=\"report.pdf\"");
}

TEST_CASE("Content disposition formatting handles ASCII filenames", "[http][content-disposition]") {
    constexpr FormattingTestCase testCases[] = {
        {"report.pdf", "attachment; filename=\"report.pdf\""},
        {"annual report.pdf", "attachment; filename=\"annual report.pdf\""},
        {"README", "attachment; filename=\"README\""},
        {".gitignore", "attachment; filename=\".gitignore\""},
        {"file-name_123.txt", "attachment; filename=\"file-name_123.txt\""},
        {"file;name.txt", "attachment; filename=\"file;name.txt\""},
        {"file=name.txt", "attachment; filename=\"file=name.txt\""},
        {"file'name.txt", "attachment; filename=\"file'name.txt\""},
        {"file+name.txt", "attachment; filename=\"file+name.txt\""},
        {"a..b.txt", "attachment; filename=\"a..b.txt\""}};

    for (const auto& testCase : testCases) {
        CAPTURE(std::string(testCase.filename));

        CHECK(
            formatContentDisposition("attachment", testCase.filename) ==
            std::string(testCase.expected));
    }
}

TEST_CASE("Content disposition formatting escapes quotation marks", "[http][content-disposition]") {
    constexpr FormattingTestCase testCases[] = {
        {"report\"2026\".pdf", "attachment; filename=\"report\\\"2026\\\".pdf\""},
        {"\"report.pdf\"", "attachment; filename=\"\\\"report.pdf\\\"\""},
        {"a\"b\"c.txt", "attachment; filename=\"a\\\"b\\\"c.txt\""},
        {"\"", "attachment; filename=\"\\\"\""}};

    for (const auto& testCase : testCases) {
        CAPTURE(std::string(testCase.filename));

        CHECK(
            formatContentDisposition("attachment", testCase.filename) ==
            std::string(testCase.expected));
    }
}

TEST_CASE(
    "Content disposition formatting handles Unicode filenames",
    "[http][content-disposition]") {
    constexpr FormattingTestCase testCases[] = {
        // resume with accented characters
        {"r\xC3\xA9sum\xC3\xA9.pdf",
         "attachment; filename=\"r_sum_.pdf\"; filename*=UTF-8''r%C3%A9sum%C3%A9.pdf"},

        // cafe with an accented character
        {"caf\xC3\xA9.pdf", "attachment; filename=\"caf_.pdf\"; filename*=UTF-8''caf%C3%A9.pdf"},

        // Chinese characters
        {"\xE6\x8A\xA5\xE5\x91\x8A.pdf",
         "attachment; filename=\"__.pdf\"; filename*=UTF-8''%E6%8A%A5%E5%91%8A.pdf"},

        // Four-byte Unicode character
        {"report\xF0\x9F\x93\x84.pdf",
         "attachment; filename=\"report_.pdf\"; filename*=UTF-8''report%F0%9F%93%84.pdf"},

        // Combining character
        {"e\xCC\x81.txt", "attachment; filename=\"e_.txt\"; filename*=UTF-8''e%CC%81.txt"},

        // Mixed Unicode and ASCII characters
        {"caf\xC3\xA9 report.pdf",
         "attachment; filename=\"caf_ report.pdf\"; filename*=UTF-8''caf%C3%A9%20report.pdf"}};

    for (const auto& testCase : testCases) {
        CAPTURE(std::string(testCase.filename));

        CHECK(
            formatContentDisposition("attachment", testCase.filename) ==
            std::string(testCase.expected));
    }
}

TEST_CASE(
    "Content disposition formatting handles percent characters",
    "[http][content-disposition]") {
    constexpr FormattingTestCase testCases[] = {
        {"report%20.pdf",
         "attachment; filename=\"report_20.pdf\"; filename*=UTF-8''report%2520.pdf"},

        {"100%.txt", "attachment; filename=\"100_.txt\"; filename*=UTF-8''100%25.txt"},

        {"%.txt", "attachment; filename=\"_.txt\"; filename*=UTF-8''%25.txt"},

        {"a%%b.txt", "attachment; filename=\"a__b.txt\"; filename*=UTF-8''a%25%25b.txt"},

        {"caf\xC3\xA9%.pdf",
         "attachment; filename=\"caf__.pdf\"; filename*=UTF-8''caf%C3%A9%25.pdf"}};

    for (const auto& testCase : testCases) {
        CAPTURE(std::string(testCase.filename));

        CHECK(
            formatContentDisposition("attachment", testCase.filename) ==
            std::string(testCase.expected));
    }
}

TEST_CASE(
    "Content disposition formatting applies extended filename encoding",
    "[http][content-disposition]") {
    constexpr FormattingTestCase testCases[] = {
        {"a b%.txt", "attachment; filename=\"a b_.txt\"; filename*=UTF-8''a%20b%25.txt"},

        {"a\"b%.txt", "attachment; filename=\"a\\\"b_.txt\"; filename*=UTF-8''a%22b%25.txt"},

        {"a'b%.txt", "attachment; filename=\"a'b_.txt\"; filename*=UTF-8''a%27b%25.txt"},

        {"a*b%.txt", "attachment; filename=\"a*b_.txt\"; filename*=UTF-8''a%2Ab%25.txt"},

        {"a;b%.txt", "attachment; filename=\"a;b_.txt\"; filename*=UTF-8''a%3Bb%25.txt"},

        {"a=b%.txt", "attachment; filename=\"a=b_.txt\"; filename*=UTF-8''a%3Db%25.txt"},

        {"a:b?c@d[e](f)%.txt",
         "attachment; filename=\"a:b?c@d[e](f)_.txt\"; "
         "filename*=UTF-8''a%3Ab%3Fc%40d%5Be%5D%28f%29%25.txt"},

        {"AZaz09!#$&+-.^_`|~%.txt",
         "attachment; filename=\"AZaz09!#$&+-.^_`|~_.txt\"; "
         "filename*=UTF-8''AZaz09!#$&+-.^_`|~%25.txt"}};

    for (const auto& testCase : testCases) {
        CAPTURE(std::string(testCase.filename));

        CHECK(
            formatContentDisposition("attachment", testCase.filename) ==
            std::string(testCase.expected));
    }
}

TEST_CASE(
    "Content disposition formatting supports valid UTF-8 boundary values",
    "[http][content-disposition]") {
    constexpr FormattingTestCase testCases[] = {
        // U+0080
        {"\xC2\x80.txt", "attachment; filename=\"_.txt\"; filename*=UTF-8''%C2%80.txt"},

        // U+07FF
        {"\xDF\xBF.txt", "attachment; filename=\"_.txt\"; filename*=UTF-8''%DF%BF.txt"},

        // U+0800
        {"\xE0\xA0\x80.txt", "attachment; filename=\"_.txt\"; filename*=UTF-8''%E0%A0%80.txt"},

        // U+D7FF
        {"\xED\x9F\xBF.txt", "attachment; filename=\"_.txt\"; filename*=UTF-8''%ED%9F%BF.txt"},

        // U+E000
        {"\xEE\x80\x80.txt", "attachment; filename=\"_.txt\"; filename*=UTF-8''%EE%80%80.txt"},

        // U+FFFF
        {"\xEF\xBF\xBF.txt", "attachment; filename=\"_.txt\"; filename*=UTF-8''%EF%BF%BF.txt"},

        // U+10000
        {"\xF0\x90\x80\x80.txt",
         "attachment; filename=\"_.txt\"; filename*=UTF-8''%F0%90%80%80.txt"},

        // U+10FFFF
        {"\xF4\x8F\xBF\xBF.txt",
         "attachment; filename=\"_.txt\"; filename*=UTF-8''%F4%8F%BF%BF.txt"}};

    for (const auto& testCase : testCases) {
        CAPTURE(std::string(testCase.filename));

        CHECK(
            formatContentDisposition("attachment", testCase.filename) ==
            std::string(testCase.expected));
    }
}

TEST_CASE(
    "Content disposition formatting rejects empty and special filenames",
    "[http][content-disposition]") {
    constexpr std::string_view invalidValues[] = {"", ".", ".."};

    for (const auto value : invalidValues) {
        CAPTURE(std::string(value));

        CHECK_THROWS_AS(formatContentDisposition("attachment", value), std::invalid_argument);
    }
}

TEST_CASE("Content disposition formatting rejects path separators", "[http][content-disposition]") {
    constexpr std::string_view invalidValues[] = {
        "/",
        "\\",
        "/report.pdf",
        "\\report.pdf",
        "directory/report.pdf",
        "directory\\report.pdf",
        "../report.pdf",
        "..\\report.pdf",
        "C:\\report.pdf",
        "report.pdf/"};

    for (const auto value : invalidValues) {
        CAPTURE(std::string(value));

        CHECK_THROWS_AS(formatContentDisposition("attachment", value), std::invalid_argument);
    }
}

TEST_CASE(
    "Content disposition formatting rejects ASCII control characters",
    "[http][content-disposition]") {
    for (unsigned int codePoint = 0; codePoint <= 0x1F; ++codePoint) {
        CAPTURE(codePoint);

        std::string filename = "report";
        filename += static_cast<char>(codePoint);
        filename += ".pdf";

        CHECK_THROWS_AS(formatContentDisposition("attachment", filename), std::invalid_argument);
    }

    CHECK_THROWS_AS(
        formatContentDisposition("attachment", "report\x7F.pdf"),
        std::invalid_argument);
}

TEST_CASE(
    "Content disposition formatting rejects header injection attempts",
    "[http][content-disposition]") {
    constexpr std::string_view invalidValues[] = {
        "report.pdf\r\nX-Injected: true",
        "report.pdf\nX-Injected: true",
        "report.pdf\rX-Injected: true",
        "report.pdf\r\n\r\n",
        "report.pdf\t"};

    for (const auto value : invalidValues) {
        CAPTURE(std::string(value));

        CHECK_THROWS_AS(formatContentDisposition("attachment", value), std::invalid_argument);
    }
}

TEST_CASE(
    "Content disposition formatting rejects invalid UTF-8 leading bytes",
    "[http][content-disposition]") {
    constexpr std::string_view invalidValues[] = {
        "\x80.txt",
        "\xBF.txt",
        "\xC0.txt",
        "\xC1.txt",
        "\xF5.txt",
        "\xF8.txt",
        "\xFE.txt",
        "\xFF.txt"};

    for (const auto value : invalidValues) {
        CAPTURE(std::string(value));

        CHECK_THROWS_AS(formatContentDisposition("attachment", value), std::invalid_argument);
    }
}

TEST_CASE(
    "Content disposition formatting rejects invalid UTF-8 continuation bytes",
    "[http][content-disposition]") {
    constexpr std::string_view invalidValues[] = {
        "\xC2"
        "A.txt",
        "\xE0\xA0"
        "A.txt",
        "\xE0"
        "A"
        "\x80.txt",
        "\xF0\x90\x80"
        "A.txt",
        "\xF0\x90"
        "A"
        "\x80.txt",
        "\xF0"
        "A"
        "\x80\x80.txt"};

    for (const auto value : invalidValues) {
        CAPTURE(std::string(value));

        CHECK_THROWS_AS(formatContentDisposition("attachment", value), std::invalid_argument);
    }
}

TEST_CASE(
    "Content disposition formatting rejects truncated UTF-8 sequences",
    "[http][content-disposition]") {
    constexpr std::string_view invalidValues[] =
        {"\xC2", "\xE0", "\xE0\xA0", "\xF0", "\xF0\x90", "\xF0\x90\x80", "report\xC3"};

    for (const auto value : invalidValues) {
        CAPTURE(std::string(value));

        CHECK_THROWS_AS(formatContentDisposition("attachment", value), std::invalid_argument);
    }
}

TEST_CASE(
    "Content disposition formatting rejects overlong UTF-8 encodings",
    "[http][content-disposition]") {
    constexpr std::string_view invalidValues[] = {
        "\xC0\x80.txt",
        "\xC0\xAF.txt",
        "\xC1\xBF.txt",
        "\xE0\x80\x80.txt",
        "\xE0\x9F\xBF.txt",
        "\xF0\x80\x80\x80.txt",
        "\xF0\x8F\xBF\xBF.txt"};

    for (const auto value : invalidValues) {
        CAPTURE(std::string(value));

        CHECK_THROWS_AS(formatContentDisposition("attachment", value), std::invalid_argument);
    }
}

TEST_CASE(
    "Content disposition formatting rejects invalid Unicode code points",
    "[http][content-disposition]") {
    constexpr std::string_view invalidValues[] = {
        // UTF-16 surrogate code points
        "\xED\xA0\x80.txt",
        "\xED\xAF\xBF.txt",
        "\xED\xB0\x80.txt",
        "\xED\xBF\xBF.txt",

        // Beyond U+10FFFF
        "\xF4\x90\x80\x80.txt",
        "\xF4\xBF\xBF\xBF.txt"};

    for (const auto value : invalidValues) {
        CAPTURE(std::string(value));

        CHECK_THROWS_AS(formatContentDisposition("attachment", value), std::invalid_argument);
    }
}

TEST_CASE(
    "Content disposition formatting applies the same rules to inline filenames",
    "[http][content-disposition]") {
    CHECK(
        formatContentDisposition("inline", "caf\xC3\xA9.pdf") ==
        "inline; filename=\"caf_.pdf\"; filename*=UTF-8''caf%C3%A9.pdf");

    CHECK(
        formatContentDisposition("inline", "report%20.pdf") ==
        "inline; filename=\"report_20.pdf\"; filename*=UTF-8''report%2520.pdf");

    CHECK(formatContentDisposition("inline", "a\"b.pdf") == "inline; filename=\"a\\\"b.pdf\"");

    CHECK_THROWS_AS(formatContentDisposition("inline", ""), std::invalid_argument);

    CHECK_THROWS_AS(formatContentDisposition("inline", "../report.pdf"), std::invalid_argument);

    CHECK_THROWS_AS(formatContentDisposition("inline", "\xFF.txt"), std::invalid_argument);
}
