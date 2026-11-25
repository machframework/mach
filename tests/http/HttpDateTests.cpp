#include <catch2/catch_test_macros.hpp>

#include <chrono>
#include <string>

#include "utility/HttpDate.hpp"

namespace
{
    using namespace std::chrono;
    using mach::detail::formatHttpDate;
}

TEST_CASE("HTTP date formatter produces IMF-fixdate values", "[http][date]") {
    const auto time = sys_days{2026y / November / 25} + hours{13} + minutes{20} + seconds{45};

    CHECK(formatHttpDate(time) == "Wed, 25 Nov 2026 13:20:45 GMT");
}

TEST_CASE("HTTP date formatter pads single-digit components", "[http][date]") {
    const auto time = sys_days{2026y / January / 2} + hours{3} + minutes{4} + seconds{5};

    CHECK(formatHttpDate(time) == "Fri, 02 Jan 2026 03:04:05 GMT");
}

TEST_CASE("HTTP date formatter handles all weekdays", "[http][date]") {
    constexpr const char* expected[] = {
        "Sun, 22 Nov 2026 00:00:00 GMT",
        "Mon, 23 Nov 2026 00:00:00 GMT",
        "Tue, 24 Nov 2026 00:00:00 GMT",
        "Wed, 25 Nov 2026 00:00:00 GMT",
        "Thu, 26 Nov 2026 00:00:00 GMT",
        "Fri, 27 Nov 2026 00:00:00 GMT",
        "Sat, 28 Nov 2026 00:00:00 GMT"};

    const auto start = sys_days{2026y / November / 22};

    for (int i = 0; i < 7; ++i) {
        CAPTURE(i);

        CHECK(formatHttpDate(start + days{i}) == expected[i]);
    }
}

TEST_CASE("HTTP date formatter handles all months", "[http][date]") {
    constexpr const char* expected[] = {
        "Thu, 01 Jan 2026 00:00:00 GMT",
        "Sun, 01 Feb 2026 00:00:00 GMT",
        "Sun, 01 Mar 2026 00:00:00 GMT",
        "Wed, 01 Apr 2026 00:00:00 GMT",
        "Fri, 01 May 2026 00:00:00 GMT",
        "Mon, 01 Jun 2026 00:00:00 GMT",
        "Wed, 01 Jul 2026 00:00:00 GMT",
        "Sat, 01 Aug 2026 00:00:00 GMT",
        "Tue, 01 Sep 2026 00:00:00 GMT",
        "Thu, 01 Oct 2026 00:00:00 GMT",
        "Sun, 01 Nov 2026 00:00:00 GMT",
        "Tue, 01 Dec 2026 00:00:00 GMT"};

    for (unsigned i = 1; i <= 12; ++i) {
        CAPTURE(i);

        CHECK(formatHttpDate(sys_days{2026y / month{i} / 1}) == expected[i - 1]);
    }
}

TEST_CASE("HTTP date formatter handles leap days", "[http][date]") {
    const auto time = sys_days{2024y / February / 29};

    CHECK(formatHttpDate(time) == "Thu, 29 Feb 2024 00:00:00 GMT");
}

TEST_CASE("HTTP date formatter handles calendar transitions", "[http][date]") {
    const auto time = sys_days{2026y / December / 31} + hours{23} + minutes{59} + seconds{59};

    CHECK(formatHttpDate(time) == "Thu, 31 Dec 2026 23:59:59 GMT");
    CHECK(formatHttpDate(time + seconds{1}) == "Fri, 01 Jan 2027 00:00:00 GMT");
}

TEST_CASE("HTTP date formatter rounds down fractional seconds", "[http][date]") {
    const auto time = sys_days{2026y / November / 25} + hours{13} + minutes{20} + seconds{45};

    CHECK(formatHttpDate(time + milliseconds{1}) == "Wed, 25 Nov 2026 13:20:45 GMT");

    CHECK(formatHttpDate(time + milliseconds{500}) == "Wed, 25 Nov 2026 13:20:45 GMT");

    CHECK(formatHttpDate(time + milliseconds{999}) == "Wed, 25 Nov 2026 13:20:45 GMT");
}

TEST_CASE("HTTP date formatter handles the Unix epoch", "[http][date]") {
    CHECK(formatHttpDate(system_clock::time_point{}) == "Thu, 01 Jan 1970 00:00:00 GMT");
}

TEST_CASE("HTTP date formatter handles dates before the Unix epoch", "[http][date]") {
    const auto time = sys_days{1969y / December / 31} + hours{23} + minutes{59} + seconds{59};

    CHECK(formatHttpDate(time) == "Wed, 31 Dec 1969 23:59:59 GMT");

    CHECK(
        formatHttpDate(system_clock::time_point{} - milliseconds{1}) ==
        "Wed, 31 Dec 1969 23:59:59 GMT");
}
