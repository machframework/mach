#include <catch2/catch_test_macros.hpp>
#include <mach/detail/http/MediaType.hpp>

#include <optional>
#include <string>
#include <string_view>

namespace
{
    using namespace mach::detail::http;

    void checkView(
        std::optional<std::string_view> actual,
        std::optional<std::string_view> expected) {
        REQUIRE(actual.has_value() == expected.has_value());
        if (expected) {
            CHECK(std::string(*actual) == std::string(*expected));
        }
    }
}

TEST_CASE("Media type extraction preserves spelling and ignores parameters", "[http][media-type]") {
    checkView(extractMediaType("application/json"), "application/json");
    checkView(extractMediaType(" \tApplication/JSON\t ; charset=utf-8"), "Application/JSON");
    checkView(extractMediaType("application/vnd.example+json"), "application/vnd.example+json");
    checkView(extractMediaType("text/plain; broken parameter"), "text/plain");
    checkView(extractMediaType("x-test/a!#$%&'*+-.^_`|~09"), "x-test/a!#$%&'*+-.^_`|~09");
}

TEST_CASE("Media type extraction rejects invalid syntax", "[http][media-type]") {
    for (std::string_view value :
         {"",
          " \t",
          "json",
          "/json",
          "application/",
          "application/json/extra",
          "application /json",
          "application/ json",
          "application/*",
          "*/json",
          "*/*",
          "application/json,text/plain",
          "application/\"json\"",
          "\napplication/json",
          "application/json\r",
          "application/json\v",
          "application/json\f",
          "application/j\xC3\xA9son",
          ";charset=utf-8"}) {
        CAPTURE(std::string(value));
        CHECK_FALSE(extractMediaType(value).has_value());
    }
    CHECK_FALSE(extractMediaType(std::string_view{"text/plain\0", 11}).has_value());
}

TEST_CASE("Charset extraction accepts token and quoted values", "[http][media-type]") {
    checkView(extractCharset("text/plain; charset=utf-8"), "utf-8");
    checkView(extractCharset("text/plain; CHARSET=UTF-8"), "UTF-8");
    checkView(extractCharset("text/plain; charset=\"ISO-8859-1\""), "ISO-8859-1");
    checkView(extractCharset("text/plain;\tcharset \t= \tutf-16 \t"), "utf-16");
    checkView(extractCharset("text/plain; foo=bar; charset=utf-8; x=y"), "utf-8");
    checkView(extractCharset("text/plain;; charset=utf-8; ;"), "utf-8");
    checkView(extractCharset("text/plain; foo=\"\"; charset=utf-8"), "utf-8");
}

TEST_CASE("Charset extraction respects quoted parameter boundaries", "[http][media-type]") {
    checkView(extractCharset(R"(text/plain; foo="a;b"; charset=utf-8)"), "utf-8");
    checkView(extractCharset(R"(text/plain; foo="a\";b"; charset=utf-8)"), "utf-8");
    checkView(extractCharset(R"(text/plain; foo="a\\b"; charset=utf-8)"), "utf-8");
    CHECK_FALSE(extractCharset(R"(text/plain; foo="a;charset=utf-8")").has_value());
    CHECK_FALSE(extractCharset("text/plain; x-charset=utf-8").has_value());
    CHECK_FALSE(extractCharset("text/plain").has_value());
}

TEST_CASE("Charset extraction rejects malformed or ambiguous parameters", "[http][media-type]") {
    for (std::string_view value :
         {"invalid; charset=utf-8",
          "text/plain; charset",
          "text/plain; charset=",
          "text/plain; charset=\"\"",
          "text/plain; charset=\"utf-8",
          "text/plain; charset=\"utf 8\"",
          "text/plain; charset=\"utf\\-8\"",
          "text/plain; charset=utf-8 extra",
          "text/plain; charset=\"utf-8\"extra",
          "text/plain; charset=utf-8; CHARSET=utf-8",
          "text/plain; charset=utf-8; charset=utf-16",
          "text/plain; broken; charset=utf-8",
          "text/plain; charset=utf-8; broken",
          "text/plain; foo=\"unterminated; charset=utf-8",
          "text/plain; foo=\"abc\\",
          "text/plain; =x; charset=utf-8",
          "text/plain; foo=\"a\rb\"; charset=utf-8",
          "text/plain; foo=\"a\\\nb\"; charset=utf-8"}) {
        CAPTURE(std::string(value));
        CHECK_FALSE(extractCharset(value).has_value());
    }
}

TEST_CASE("Media type matching is exact and ASCII case insensitive", "[http][media-type]") {
    CHECK(matchesMediaType("Application/JSON; charset=utf-8", "application/json"));
    CHECK(matchesMediaType(" \ttext/plain\t", " TEXT/PLAIN\t"));
    CHECK(matchesMediaType("text/plain; broken", "text/plain"));
    CHECK_FALSE(matchesMediaType("application/problem+json", "application/json"));
    CHECK_FALSE(matchesMediaType("application/jsonp", "application/json"));
    CHECK_FALSE(matchesMediaType("application/json", "application/*"));
    CHECK_FALSE(matchesMediaType("application/json", "application/json; charset=utf-8"));
    CHECK_FALSE(matchesMediaType("application/json", ""));
    CHECK_FALSE(matchesMediaType("application/json", "application /json"));
    CHECK_FALSE(matchesMediaType("application/json", "application/json\n"));
    CHECK_FALSE(matchesMediaType("invalid", "invalid"));
}

TEST_CASE("JSON media type inspection recognizes structured suffixes", "[http][media-type]") {
    for (std::string_view value :
         {"application/json",
          "APPLICATION/JSON",
          "application/json; charset=utf-8",
          "application/problem+json",
          "application/vnd.api+JSON",
          "model/example+json",
          "application/json; broken"}) {
        CAPTURE(std::string(value));
        CHECK(isJsonMediaType(value));
    }
    for (std::string_view value :
         {"",
          "text/json",
          "application/jsonp",
          "application/+json",
          "application/json+other",
          "application/example+jsonx",
          "application/*",
          "application /json",
          "application/json/extra"}) {
        CAPTURE(std::string(value));
        CHECK_FALSE(isJsonMediaType(value));
    }
}

TEST_CASE("JSON charset validation accepts absent or UTF-8 charset", "[http][media-type]") {
    for (std::string_view value :
         {"application/json",
          "Application/JSON; CHARSET=UTF-8",
          "application/json; charset=\"utf-8\"",
          "application/json; charset = utf-8",
          "application/json;;",
          "application/json; foo=bar",
          "application/json; foo=\"charset=utf-16\"",
          "application/json; foo=\"a;b\"; charset=utf-8"}) {
        CAPTURE(std::string(value));
        CHECK_FALSE(hasUnsupportedJsonCharset(value));
    }
}

TEST_CASE(
    "JSON charset validation rejects unsupported or malformed charset",
    "[http][media-type]") {
    for (std::string_view value :
         {"invalid",
          "application/json; charset=utf-16",
          "application/json; charset=iso-8859-1",
          "application/json; charset=utf8",
          "application/json; charset=",
          "application/json; charset",
          "application/json; charset=\"\"",
          "application/json; charset=\"utf-8",
          "application/json; charset=\"utf\\-8\"",
          "application/json; charset=utf-8; CHARSET=utf-8",
          "application/json; charset=utf-8; broken",
          "application/json; foo=\"unterminated"}) {
        CAPTURE(std::string(value));
        CHECK(hasUnsupportedJsonCharset(value));
    }
}
