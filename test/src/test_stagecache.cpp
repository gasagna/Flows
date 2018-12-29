#include <cmath>
#include <iostream>
#include <valarray>

#include "Flows.hpp"
#include "catch.hpp"
#include "testlorenz.hpp"

using namespace Flows;

TEST_CASE("stagecache", "tests") {

    SECTION("rk4") {

        /* DEFINE NONLINEAR OBJECT TO FILL THE STAGE CACHE */
        // initial condition
        vec3 x = { 1.0, 1.0, 2.0 };

        // define nonlinear forward method
        auto mx = RK4<vec3, false>(x);

        // construct system
        auto a     = Lorenz(0.0);
        auto noop  = NoOpFunction();
        auto sys_x = System(a, noop);

        // define cache
        auto cache = RAMStageCache<vec3, 4>();

        // call object and fill cache for one step
        step(mx, sys_x, 0.0, 1e-2, x, cache);

        /* DEFINE FORWARD LINEAR PROBLEM */
        vec3 y = { 1.0, 2.0, 3.0 };
        x      = { 1.0, 1.0, 2.0 }; // reset state

        auto z_copy = couple(x, y);
        auto z_ref  = refcouple(x, y);

        // define linearised forward method
        auto mz = RK4<Pair<vec3, vec3>, false>(z_copy);

        // construct system
        auto a_tan = LorenzTan(0.0);
        auto sys_z = System(std::forward_as_tuple(a, a_tan), std::forward_as_tuple(noop, noop));

        // call object
        step(mz, sys_z, 0.0, 0.01, z_ref, NoOpStageCache<Pair<vec3, vec3>>());

        /* DEFINE ADJOINT LINEAR PROBLEM */
        vec3 w = { 4.0, 5.0, 7.0 };

        // define linearised forward method
        auto mw = RK4<vec3, true>(w);

        // construct system
        auto a_adj = LorenzAdj(0.0);
        auto sys_w = System(a_adj, noop);

        // get stages from the cache
        auto [t, dt, stages] = cache[0];

        // call object
        step(mw, sys_w, 0.0, 0.01, w, stages);

        // calculate dot products
        auto p_1 = y[0] * 4.0 + y[1] * 5.0 + y[2] * 7.0;
        auto p_2 = w[0] * 1.0 + w[1] * 2.0 + w[2] * 3.0;

        REQUIRE(std::fabs(p_1 - p_2) / std::fabs(p_1) < 1e-14);
    }

    SECTION("CB3R2R") {

        /* DEFINE NONLINEAR OBJECT TO FILL THE STAGE CACHE */
        // initial condition
        vec3 x = { 15.0, 16.0, 20.0 };

        // define nonlinear forward method
        auto mx = CB3R2R_3E<vec3, false>(x);

        // construct system
        auto a     = Lorenz(1);
        auto sys_x = System(a, a);

        // define cache
        auto cache = RAMStageCache<vec3, 4>();

        // call object and fill cache for one step
        step(mx, sys_x, 0.0, 1e-2, x, cache);


        /* DEFINE FORWARD LINEAR PROBLEM */
        vec3 y = { 1.0, 2.0, 3.0 };
        x      = { 15.0, 16.0, 20.0 }; // reset state

        auto z_copy = couple(x, y);
        auto z_ref  = refcouple(x, y);

        // define linearised forward method
        auto mz = CB3R2R_3E<Pair<vec3, vec3>, false>(z_copy);

        // construct system
        auto a_tan = LorenzTan(1);
        auto sys_z = System(std::forward_as_tuple(a, a_tan), std::forward_as_tuple(a, a_tan));

        // call object
        step(mz, sys_z, 0.0, 0.01, z_ref, NoOpStageCache<Pair<vec3, vec3>>());


        /* DEFINE ADJOINT LINEAR PROBLEM */
        vec3 w = { 4.0, 5.0, 7.0 };

        // define linearised forward method
        auto mw = CB3R2R_3E<vec3, true>(w);

        // construct system
        auto a_adj = LorenzAdj(1);
        auto sys_w = System(a_adj, a_adj);

        // get stages from the cache
        auto [t, dt, stages] = cache[0];

        // call object
        step(mw, sys_w, 0.0, 0.01, w, stages);

        // calculate dot products
        auto p_1 = y[0] * 4.0 + y[1] * 5.0 + y[2] * 7.0;
        auto p_2 = w[0] * 1.0 + w[1] * 2.0 + w[2] * 3.0;

        REQUIRE(std::fabs(p_1 - p_2) / std::fabs(p_1) < 1e-14);
    }
}