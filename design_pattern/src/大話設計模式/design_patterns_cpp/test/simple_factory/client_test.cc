#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "simple_factory/client.h"

TEST_CASE("Calculate client test", "[client]")
{
    REQUIRE(3 == calculate(1, 2, "+"));
    REQUIRE(0 == calculate(1, 1, "-"));
    REQUIRE(2 == calculate(1, 2, "*"));
    REQUIRE(2 == calculate(6, 3, "/"));
    REQUIRE_THROWS(calculate(6, 0, "/"));
}
