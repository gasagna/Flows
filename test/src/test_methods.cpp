#include <cmath>

#include "Flows.hpp"
#include "catch.hpp"
#include "testsystem.hpp"

using namespace Flows;

TEST_CASE("methods", "tests") {

    SECTION("CB3R2R") {
        // initial condition
        double z0 = 1.0;

        // define system
        ImplicitTerm imTerm(0.5);
        ExplicitTerm exTerm(0.5);

        // define method
        auto m1 = CB3R2R_3E(z0);
        auto m2 = CB3R2R_2(z0);

        // construct system
        auto sys = System(exTerm, imTerm);

        // define time stepping
        auto stepping = TimeStepConstant(1);

        // define integrator
        auto phi1 = Flow(sys, m1, stepping);
        auto phi2 = Flow(sys, m2, stepping);

        for (double dt : { 1e-1, 1e-2, 1e-3 }) {
            z0          = 1.0;
            stepping.dt = dt;
            REQUIRE(std::fabs(phi1(z0, 0, 1) - std::exp(1)) / std::pow(dt, 3) < 0.019);

            z0          = 1.0;
            stepping.dt = dt;
            REQUIRE(std::fabs(phi2(z0, 0, 1) - std::exp(1)) / std::pow(dt, 2) < 0.068);
        }
    }

    SECTION("CNRK2") {
        // initial condition
        double z0 = 1.0;

        // define system
        ImplicitTerm imTerm(0.5);
        ExplicitTerm exTerm(0.5);

        // define method
        auto m = CNRK2(z0);

        // construct system
        auto sys = System(exTerm, imTerm);

        // define time stepping
        auto stepping = TimeStepConstant(1);

        // define integrator
        auto phi = Flow(sys, m, stepping);

        for (double dt : { 1e-1, 1e-2, 1e-3, 1e-4 }) {
            z0          = 1.0;
            stepping.dt = dt;
            REQUIRE(std::fabs(phi(z0, 0, 1) - std::exp(1)) / std::pow(dt, 2) < 0.057);
        }
    }

    SECTION("RK4") {
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
        auto stepping = TimeStepConstant(1);

        // define integrator
        auto phi = Flow(sys, m, stepping);

        for (double dt : { 1e-1, 1e-2, 1e-3 }) {
            z0          = 1.0;
            stepping.dt = dt;
            REQUIRE(std::fabs(phi(z0, 0, 1) - std::exp(1)) / std::pow(dt, 4) < 0.023);
        }
    }
}