#include "catch.hpp"
#include <Flows.hpp>
#include <cmath>
#include <iostream>
#include <tuple>
#include <valarray>

TEST_CASE("Testing Joint", "[vector and scalar]") {

    // initial condition
    std::valarray<double> x1 = { 1.0 };
    std::valarray<double> y1 = { 2.0 };

    std::valarray<double> x2 = { 3.0 };
    std::valarray<double> y2 = { 4.0 };

    std::valarray<double> x3 = { 0.0 };
    std::valarray<double> y3 = { 0.0 };

    std::valarray<double> x4 = { 0.0 };
    std::valarray<double> y4 = { 0.0 };

    double q1 = 5;
    double q2 = 6;
    double q3 = 0;
    double q4 = 0;
    auto   a  = Flows::couple(x1, y1, q1);
    auto   b  = Flows::couple(x2, y2, q2);
    auto   c  = Flows::couple(x3, y3, q3);

    c  = 2 * a + 3 * b - a * 2 + b / 1.5;
    x4 = 2 * x1 + 3 * x2 - x1 * 2 + x2 / 1.5;
    y4 = 2 * y1 + 3 * y2 - y1 * 2 + y2 / 1.5;
    q4 = 2 * q1 + 3 * q2 - q1 * 2 + q2 / 1.5;

    REQUIRE(Flows::get<0>(c, 0) == x4[0]);
    REQUIRE(Flows::get<1>(c, 0) == y4[0]);
    REQUIRE(Flows::get<2>(c) == q4);

    // set to value
    c = 0;
    REQUIRE(Flows::get<0>(c, 0) == 0);
    REQUIRE(Flows::get<1>(c, 0) == 0);
    REQUIRE(Flows::get<2>(c) == 0);
}