#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "strategy/client.h"

TEST_CASE("Calculate client test", "[client]")
{
    REQUIRE(50 == calculate(5, 10, "normal"));
    REQUIRE(45 == calculate(5, 10, "rebate 9"));
    REQUIRE(40 == calculate(5, 10, "return 10 by 50"));
    REQUIRE(40 == calculate(5, 10, "denote 1 by 5"));
}
