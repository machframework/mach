#include <catch2/catch_test_macros.hpp>

#include <cstdint>
#include <limits>
#include <stdexcept>

#include <mach/AppBuilder.hpp>

TEST_CASE("AppBuilder validates configured port values", "[app_builder][validation]") {
    SECTION("Valid ports are accepted") {
        for (const std::uint32_t port : {1u, 3143u, 8080u, 65535u}) {
            CAPTURE(port);

            REQUIRE_NOTHROW(mach::AppBuilder().port(port).build());
        }
    }

    SECTION("Invalid ports are rejected") {
        for (const std::uint32_t port :
             {0u, 65536u, 65537u, 100000u, std::numeric_limits<std::uint32_t>::max()}) {
            CAPTURE(port);

            REQUIRE_THROWS_AS(mach::AppBuilder().port(port).build(), std::invalid_argument);
        }
    }

    SECTION("Invalid ports supplied through configureApp are rejected") {
        mach::AppBuilder builder;

        builder.configureApp([](mach::AppOptions& options) {
            options.port = 65537;
        });

        REQUIRE_THROWS_AS(builder.build(), std::invalid_argument);
    }
}
