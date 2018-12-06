#include "catch.hpp"
#include <Flows.hpp>
#include <valarray>
#include <iostream>
#include <tuple>
#include <cmath>
#include <chrono>

TEST_CASE("Testing Joint", "[vector and scalar]") {
    
    // initial condition
    std::valarray<double> x1(2.0, 100000);
    std::valarray<double> x2(2.0, 100000);
    std::valarray<double> x3(2.0, 100000);
    std::valarray<double> x4(2.0, 100000);
    std::valarray<double> y1(2.0, 100000);
    std::valarray<double> y2(2.0, 100000);
    std::valarray<double> y3(2.0, 100000);
    std::valarray<double> y4(2.0, 100000);

    auto a = Flows::couple(x1, y1);
    auto b = Flows::couple(x2, y2);
    auto c = Flows::couple(x3, y3);

    using clock = std::chrono::high_resolution_clock;
    clock::time_point start, end;
    clock::duration elapsed;

    double N = 5000; 

    double j = 0;
    double tmin = 100000000;
    for (auto i = 0; i<N; i++) {
        start = clock::now();
        c  = 2*a + 3*b - a*2 + b/1.5;
        end = clock::now();
        elapsed = end - start;
        if (elapsed.count() < tmin)
            tmin = elapsed.count();
    }
    std::cout << tmin << " " << j << std::endl;

    // manual
    j = 0;
    tmin = 100000000;
    for (auto i = 0; i<N; i++) {
        start = clock::now();
        x4 = 2*x1 + 3*x2 - x1*2 + x2/1.5;
        y4 = 2*y1 + 3*x2 - x1*2 + x2/1.5;
        end = clock::now();
        elapsed = end - start;
        if (elapsed.count() < tmin)
            tmin = elapsed.count();
    }
    std::cout << tmin << " " << j << std::endl;
    
    // y4 = 1*y1 + 3*y2 - y1*2 + y2/1.5;

    // REQUIRE( std::get<0>(c) == x4 );
    // REQUIRE( std::get<1>(c)[0] == y4 );

    // set to value
    // c = 1;
    // REQUIRE( std::get<0>(c) == 1 );
    // REQUIRE( std::get<1>(c) == 1 );
}

// TEST_CASE("Testing Joint", "[two scalars]") {
    
//     // initial condition
//     double x1 = 4.0;
//     double y1 = 5.0;

//     double x2 = 3.0;
//     double y2 = 6.0;
    
//     double x3 = 0.0;
//     double y3 = 0.0;
    
//     auto z1 = Flows::Joint(x1, y1);
//     auto z2 = Flows::Joint(x2, y2);
//     auto z3 = Flows::Joint(x3, y3);

//     // forward over all fields
//     z3 = 2*z1 + 3*z2 - (z1*4 + z2/2);

//     REQUIRE( std::get<0>(z3) ==  2*4 + 3*3 - (4*4 + 3.0/2) );
//     REQUIRE( std::get<1>(z3) ==  2*5 + 3*6 - (5*4 + 6.0/2) );

//     // set to value
//     z3 = 1;
//     REQUIRE( std::get<0>(z3) == 1 );
//     REQUIRE( std::get<1>(z3) == 1 );
// }