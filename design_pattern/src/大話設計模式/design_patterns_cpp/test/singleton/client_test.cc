#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "singleton/hungry_singleton.h"
#include "singleton/lazy_singleton.h"
#include "singleton/lock_singleton.h"

TEST_CASE("Singleton", "[client]")
{
    SECTION("HungrySingleton") {
        HungrySingleton* single1 = HungrySingleton::instance();
        HungrySingleton* single2 = HungrySingleton::instance();
        REQUIRE(single1 != nullptr);
        REQUIRE(single1 == single2);
    }
    SECTION("LazySingleton") {
        LazySingleton* single1 = LazySingleton::instance();
        LazySingleton* single2 = LazySingleton::instance();
        REQUIRE(single1 != nullptr);
        REQUIRE(single1 == single2);
    }
    SECTION("LockSingleton") {
        LockSingleton* single1 = LockSingleton::instance();
        LockSingleton* single2 = LockSingleton::instance();
        REQUIRE(single1 != nullptr);
        REQUIRE(single1 == single2);
    }
}
