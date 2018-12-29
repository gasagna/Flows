#include <cmath>
#include <iostream>
#include <valarray>

#include "Flows.hpp"
#include "catch.hpp"
#include "testsystem.hpp"

using namespace Flows;

// integrate dqdt = x
auto quad =
    [](double t, const double& x,
        const double& dxdt, const double& q,
        double& dqdt) {
        dqdt = x;
    };

TEST_CASE("coupled", "tests") {

    SECTION("integration with quadrature") {

        // initial condition
        double x = 1.0;
        double q = 0.0;

        // define system
        ExplicitTerm exTerm(0.5);
        ImplicitTerm imTerm(0.5);

        // define method
        auto z1 = couple(0.0, 0.0);
        auto m  = CB3R2R_3E(z1);

        // construct system
        auto noop = NoOpFunction();
        auto sys  = System(std::forward_as_tuple(exTerm, quad),
            std::forward_as_tuple(imTerm, noop));

        // define time stepping
        auto stepping = TimeStepConstant(1e-4);

        // define integrator
        auto phi = Flow(sys, m, stepping);

        // propagate these two doubles
        auto z2 = refcouple(x, q);

        // monitor the x state of the pair. Note that the lambda returns by value
        // so a copy of x is made at every time the lambda is called. This is
        // useful in general, because when we propagate a pair, we keep updating
        // the same object in place. If this was not a copy we would have a bunch
        // of objects pointing to the same memory.
        auto mon = Monitor(z2, RAMStorage<double>(),
            [](const Pair<double&, double&>& xq) { return std::get<0>(xq); }, 5000);

        // call object
        phi(z2, 0.0, 1.0, mon);

        REQUIRE(mon.samples()[0] - std::exp(0.0) < 1e-12);
        REQUIRE(mon.samples()[1] - std::exp(0.5) < 1e-12);
        REQUIRE(mon.samples()[2] - std::exp(1.0) < 1e-12);

        // the solution is exp(1)
        REQUIRE(std::fabs(std::get<0>(z2) - std::exp(1.0)) < 1e-12);

        // the integral of exp(t) from 0 to 1 is exp(1) - exp(0)
        REQUIRE(std::fabs(std::get<1>(z2) - std::exp(1.0) + 1.0) < 1e-12);

        // Test another monitor. Here we want to store the full pair, 
        // so we need to strip down the references from the two doubles
        // in the Pair and then pass RefToCopy() as a functor to make a 
        // copy of the data.
        auto mon2 = Monitor(z2, RAMStorage<Pair<double, double>>(),
            Copy(), 5000);

        // reset initial condition
        x = 1.0;
        q = 0.0;

        // call object
        phi(z2, 0.0, 1.0, mon2);

        REQUIRE(std::get<0>(mon2.samples()[0]) - std::exp(0.0) < 1e-12);
        REQUIRE(std::get<0>(mon2.samples()[1]) - std::exp(0.5) < 1e-12);
        REQUIRE(std::get<0>(mon2.samples()[2]) - std::exp(1.0) < 1e-12);
    }

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