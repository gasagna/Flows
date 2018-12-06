#include "catch.hpp"
#include <Flows.hpp>
#include <valarray>
#include <iostream>
#include <tuple>
#include <cmath>

TEST_CASE("Testing RAMMonitor", "[monitor]") {
    // push this
    std::valarray<double> z0 = {1.0};

    // define monitor
    Flows::RAMMonitor<std::valarray<double>> mon;

    // push stuff
    mon.push_back(0.0, {1.0});
    mon.push_back(1.0, {2.0});
    mon.push_back(2.0, {3.0});

    // basic tests
    REQUIRE( mon.times()[0] == 0.0 );
    REQUIRE( mon.times()[1] == 1.0 );
    REQUIRE( mon.times()[2] == 2.0 );
    REQUIRE( mon.samples()[0][0] == 1.0 );
    REQUIRE( mon.samples()[1][0] == 2.0 );
    REQUIRE( mon.samples()[2][0] == 3.0 );
}