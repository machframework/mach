#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <mach/byte_literals.hpp>

using namespace mach::literals;

TEST_CASE("Byte literals produce correct values", "[core][literals]") {
    SECTION("Bytes") {
        STATIC_REQUIRE(512_B == 512);
    }

    SECTION("SI units") {
        STATIC_REQUIRE(1_kB == 1000);
        STATIC_REQUIRE(1_MB == 1'000'000);
        STATIC_REQUIRE(1_GB == 1'000'000'000);
        STATIC_REQUIRE(1_TB == 1'000'000'000'000ULL);
    }

    SECTION("IEC units") {
        STATIC_REQUIRE(1_KiB == 1024);
        STATIC_REQUIRE(1_MiB == 1'048'576);
        STATIC_REQUIRE(1_GiB == 1'073'741'824);
        STATIC_REQUIRE(1_TiB == 1'099'511'627'776ULL);
    }
}

TEST_CASE("Byte literals handle boundary values", "[core][literals]") {
    SECTION("Zero values") {
        STATIC_REQUIRE(0_B == 0);
        STATIC_REQUIRE(0_kB == 0);
        STATIC_REQUIRE(0_KiB == 0);
        STATIC_REQUIRE(0_TiB == 0);
    }

    SECTION("Maximum representable value") {
        STATIC_REQUIRE(18446744073709551615_B == std::numeric_limits<std::size_t>::max());
    }
}