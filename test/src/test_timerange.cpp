#include "catch.hpp"
#include <Flows.hpp>
#include <iostream>
#include <cmath>
#include <stdio.h>

TEST_CASE("Testing timerange", "[timerange]") {
    
    int      i = 0;
    double tol = 2e-16;

    SECTION ("case 1") {
        auto rng = Flows::TimeRange(0, 1, 0.4);
        std::vector<double>   ts = {0.0, 0.4, 0.8};
        std::vector<double>  dts = {0.4, 0.4, 0.2};
        REQUIRE( rng.len == 3 );
        for (auto [t, dt] : rng) {
            // printf("%.17f - %.17f - %.17f - %.17f\n", t, ts[i], dt, dts[i]);
            REQUIRE( fabs(t  -   ts[i]) < tol );
            REQUIRE( fabs(dt -  dts[i]) < tol );
            i++;
        }
    }

    SECTION ("case 2") {
        auto rng = Flows::TimeRange(0, 1, 0.2);
        std::vector<double>  ts = {0.0, 0.2, 0.4, 0.6, 0.8};
        std::vector<double> dts = {0.2, 0.2, 0.2, 0.2, 0.2};
        REQUIRE( rng.len == 5 );
        for (auto [t, dt] : rng) {
            REQUIRE( fabs(t  -   ts[i]) < tol );
            REQUIRE( fabs(dt -  dts[i]) < tol );
            i++;
        }
    }

    SECTION ("case 3") {
        auto rng = Flows::TimeRange(1, 0, 0.4);
        std::vector<double>   ts = { 1.0,  0.6,  0.2};
        std::vector<double>  dts = {-0.4, -0.4, -0.2};
        REQUIRE( rng.len == 3 );
        for (auto [t, dt] : rng) {
            REQUIRE( fabs(t  -   ts[i]) < tol );
            REQUIRE( fabs(dt -  dts[i]) < tol );
            i++;
        }
    }

    SECTION ("case 4") {
        auto rng = Flows::TimeRange(1, 0, 0.2);
        std::vector<double>  ts = { 1.0,  0.8,  0.6,  0.4,  0.2};
        std::vector<double> dts = {-0.2, -0.2, -0.2, -0.2, -0.2};
        REQUIRE( rng.len == 5 );
        for (auto [t, dt] : rng) {
            REQUIRE( fabs(t  -   ts[i]) < tol );
            REQUIRE( fabs(dt -  dts[i]) < tol );
            i++;
        }
    }

    SECTION ("case 5") {
        auto rng = Flows::TimeRange(0, 1, 1e-1);
        std::vector<double>  ts = {0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9};
        std::vector<double> dts = {0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1};
        REQUIRE( rng.len == 10 );
        for (auto [t, dt] : rng) {
            REQUIRE( fabs(t  -   ts[i]) < tol );
            REQUIRE( fabs(dt -  dts[i]) < tol );
            i++;
        }
    }

}