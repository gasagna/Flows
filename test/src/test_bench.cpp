#include "catch.hpp"

#include "Flows.hpp"

#include <valarray>
#include <cmath>
#include <chrono>

TEST_CASE("Testing Bench", "[bench]") {
    
    // initial condition
    std::valarray<double> x1(2, 10000);
    std::valarray<double> y1(2, 10000);
    
    std::valarray<double> x2(2, 10000);
    std::valarray<double> y2(2, 10000);
    
    std::valarray<double> x3(2, 10000);
    std::valarray<double> y3(2, 10000);
    
    std::valarray<double> x4(2, 10000);
    std::valarray<double> y4(2, 10000);

    double q1 = 5;
    double q2 = 6;
    double q3 = 0;
    double q4 = 0;
    auto a = Flows::couple(x1, y1, q1);
    auto b = Flows::couple(x2, y2, q2);
    auto c = Flows::couple(x3, y3, q3);


    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i != 10000; i++) {
        c  = 2*a  + 3*b  - a*2  + b/1.5;
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_1 = end - start;
    

    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i != 10000; i++) {
        x4 = 2*x1 + 3*x2 - x1*2 + x2/1.5;
        y4 = 2*y1 + 3*y2 - y1*2 + y2/1.5;
        q4 = 2*q1 + 3*q2 - q1*2 + q2/1.5;
    }
    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_2 = end - start;

    // difference in time must be < than 5%, mostly due to noise
    REQUIRE( std::fabs(elapsed_1.count() - elapsed_2.count())/elapsed_1.count() < 0.05);
}