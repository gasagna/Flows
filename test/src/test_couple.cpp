#include "cxxabi.h"
#include <cmath>
#include <iostream>
#include <valarray>

#define DEMANGLE_TYPEID_NAME(x) abi::__cxa_demangle(typeid((x)).name(), NULL, NULL, NULL)

#include "Flows.hpp"
#include "catch.hpp"
#include "testsystem.hpp"

using namespace Flows;

TEST_CASE("coupled", "tests") {

    // SECTION("integration") {

    //     // initial condition
    //     double x = 1.0;
    //     double y = 1.0;
    //     double w = 1.0;

    //     // define system
    //     ExplicitTerm exTerm1(0.5);
    //     ImplicitTerm imTerm1(0.5);
    //     ExplicitTerm exTerm2(0.5);
    //     ImplicitTerm imTerm2(0.5);

    //     // define method
    //     auto z1 = couple(1.0, 1.0);
    //     auto z2 = refcouple(x, y);
    //     auto z3 = z1 + z2;

    //     auto m = RK4(z1);

    //     std::cout << m.storage[0].a << "\n";
    //     std::cout << m.storage[1].a << "\n";
    //     std::cout << m.storage[2].a << "\n";
    //     std::cout << m.storage[3].a << "\n";
    //     std::cout << m.storage[4].a << "\n";

    //     x = 2;

    //     std::cout << m.storage[0].a << "\n";
    //     std::cout << m.storage[1].a << "\n";
    //     std::cout << m.storage[2].a << "\n";
    //     std::cout << m.storage[3].a << "\n";
    //     std::cout << m.storage[4].a << "\n";

    //     std::cout << DEMANGLE_TYPEID_NAME(z1) << "\n";
    //     std::cout << DEMANGLE_TYPEID_NAME(z2) << "\n";
    //     std::cout << DEMANGLE_TYPEID_NAME(z3) << "\n";
    //     std::cout << DEMANGLE_TYPEID_NAME(m)  << "\n";

    //     // // construct system
    //     // auto sys = System(std::make_tuple(exTerm1, exTerm2), std::make_tuple(imTerm1, imTerm2));

    //     // // define time stepping
    //     // auto stepping = TimeStepConstant(1e-4);

    //     // // define integrator
    //     // auto phi = Flow(sys, m, stepping);

    //     // // define monitor
    //     // auto mon = Monitor(RAMStorage<double>(), Identity(), 5000);

    //     // // call object
    //     // phi(z0, 0, 1, mon);

    //     // // check against expected value
    //     // std::vector<double> ts_expected = { 0.0, 0.5, 1.0 };
    // }

    SECTION("arithmetic") {

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
        auto   a  = couple(x1, y1, q1);
        auto   b  = couple(x2, y2, q2);
        auto   c  = couple(x3, y3, q3);

        c  = 2 * a + 3 * b - a * 2 + b / 1.5;
        x4 = 2 * x1 + 3 * x2 - x1 * 2 + x2 / 1.5;
        y4 = 2 * y1 + 3 * y2 - y1 * 2 + y2 / 1.5;
        q4 = 2 * q1 + 3 * q2 - q1 * 2 + q2 / 1.5;

        REQUIRE(std::get<0>(c, 0) == x4[0]);
        REQUIRE(std::get<1>(c, 0) == y4[0]);
        REQUIRE(std::get<2>(c) == q4);

        // set to value
        c = 0;
        REQUIRE(std::get<0>(c, 0) == 0);
        REQUIRE(std::get<1>(c, 0) == 0);
        REQUIRE(std::get<2>(c) == 0);
    }

    SECTION("copy/reference") {

        double q1 = 1.0;
        double q2 = 2.0;
        double q3 = 1.0;
        double q4 = 2.0;

        // this is a copy
        auto a = couple(q1, q2);
       
        // this is made of references
        auto b = refcouple(q3, q4);

        // this makes a copy too, fro rvalues
        auto c = couple(7.0, 8.0);

        REQUIRE(std::get<0>(a) == 1.0);
        REQUIRE(std::get<1>(a) == 2.0);
        REQUIRE(std::get<0>(b) == 1.0);
        REQUIRE(std::get<1>(b) == 2.0);
        REQUIRE(std::get<0>(c) == 7.0);
        REQUIRE(std::get<1>(c) == 8.0);

        q1 = 5.0;
        q2 = 6.0;
        q3 = 5.0;
        q4 = 6.0;

        REQUIRE(std::get<0>(a) == 1.0);
        REQUIRE(std::get<1>(a) == 2.0);
        REQUIRE(std::get<0>(b) == 5.0);
        REQUIRE(std::get<1>(b) == 6.0);

        // now change pair and see what happes
        std::get<0>(a) = 11.0;
        std::get<1>(a) = 12.0;
        std::get<0>(b) = 13.0;
        std::get<1>(b) = 14.0;

        REQUIRE(q1 == 5.0);
        REQUIRE(q2 == 6.0);
        REQUIRE(q3 == 13.0);
        REQUIRE(q4 == 14.0);
    }
}