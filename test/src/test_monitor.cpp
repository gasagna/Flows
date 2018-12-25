#include <cmath>
#include <iostream>

#include "Flows.hpp"
#include "catch.hpp"
#include "testsystem.hpp"

using namespace Flows;

TEST_CASE("monitor", "tests") {

    // initial condition
    double z0 = 1.0;

    // define system
    ExplicitTerm exTerm(1.0);
    NoOpFunction imTerm{};

    // define method
    auto m = RK4(z0);

    // construct system
    auto sys = System(exTerm, imTerm);

    // define time stepping
    auto stepping = TimeStepConstant(1e-4);

    // define integrator
    auto phi = Flow(sys, m, stepping);

    // define monitor
    auto mon = Monitor(RAMStorage<double>(), Identity(), 5000);

    // call object
    phi(z0, 0, 1, mon);

    // check against expected value
    std::vector<double> ts_expected = { 0.0, 0.5, 1.0 };

    for (int i : { 0, 1, 2 }) {
        REQUIRE(mon.times()[i] == ts_expected[i]);
        REQUIRE(std::fabs(mon.samples()[i] - std::exp(ts_expected[i])) < 1e-10);
    }
}