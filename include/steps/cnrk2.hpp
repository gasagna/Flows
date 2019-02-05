#pragma once
#include "generic.hpp"
#include "stagecache.hpp"
#include "system.hpp"
#include <array>

namespace Flows {

////////////////////////////////////////////////////////////////
// Crank-Nicolson Runge-Kutta method
template <typename Y, bool ISADJOINT = false>
struct CNRK2 : public AbstractMethod<Y, 5, ISADJOINT> {
    CNRK2(const Y& x)
        : AbstractMethod<Y, 5, ISADJOINT>(x) {}
};

// forward integration
template <typename Y, typename X, typename SYSTEM, typename STAGECACHE>
void step(CNRK2<Y, false>& method,
          SYSTEM&          sys,
          double           t,
          double           dt,
          X&               x,
          STAGECACHE&&     c) {

    // aliases
    auto& k1 = method.storage[0];
    auto& k2 = method.storage[1];
    auto& k3 = method.storage[2];
    auto& k4 = method.storage[3];
    auto& k5 = method.storage[4];

    // prepare cache for new step
    c.setup_step(t, dt);

    // predictor step
    sys.ImcA_mul(k1, x,  -0.5 * dt);
    sys(t, x, k2);
    c.push_back(x);
    k3 = k1 + dt * k2;
    sys.ImcA_div(k4, k3,  0.5 * dt);

    // corrector
    sys(t + dt, k4, k5);
    c.push_back(k4);
    k3 = k1 + 0.5 * dt * (k2 + k5);
    sys.ImcA_div(x, k3, 0.5 * dt);

    c.close_step();
};
}