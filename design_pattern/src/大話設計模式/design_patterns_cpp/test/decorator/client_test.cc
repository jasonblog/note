#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "decorator/client.h"

TEST_CASE("show_finery", "[client]")
{
    show_finery();
}
