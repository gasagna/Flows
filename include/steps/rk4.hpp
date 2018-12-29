#pragma once
#include "generic.hpp"
#include "stagecache.hpp"
#include "system.hpp"
#include <array>

namespace Flows {

////////////////////////////////////////////////////////////////
// Classical fourth order Runge-Kutta method
template <typename Y, bool ISADJOINT = false>
struct RK4 : public AbstractMethod<Y, 5, ISADJOINT> {
    RK4(const Y& x)
        : AbstractMethod<Y, 5, ISADJOINT>(x) {}
};

// forward integration
template <typename Y, typename X, typename SYSTEM, typename STAGECACHE>
void step(RK4<Y, false>& method,
    SYSTEM&              sys,
    double               t,
    double               dt,
    X&                   x,
    STAGECACHE&&         c) {

    // aliases
    auto& k1 = method.storage[0];
    auto& k2 = method.storage[1];
    auto& k3 = method.storage[2];
    auto& k4 = method.storage[3];
    auto& y  = method.storage[4];

    // prepare cache for new step
    c.setup_step(t, dt);

    // stages
    y = x;
    sys(t, y, k1);
    c.push_back(y);

    y = x + dt * k1 / 2;
    sys(t + dt / 2, y, k2);
    c.push_back(y);

    y = x + dt * k2 / 2;
    sys(t + dt / 2, y, k3);
    c.push_back(y);

    y = x + dt * k3;
    sys(t + dt, y, k4);
    c.push_back(y);

    // wrap up
    x = x + dt / 6 * (k1 + 2 * k2 + 2 * k3 + k4);

    c.close_step();
};

// backward integration
template <typename Y, typename X, typename SYSTEM, typename STAGES>
void step(RK4<Y, true>& method,
    SYSTEM&             sys,
    double              t,
    double              dt,
    X&                  x,
    STAGES&&            stages) {

    // aliases
    auto& k1 = method.storage[0];
    auto& k2 = method.storage[1];
    auto& k3 = method.storage[2];
    auto& k4 = method.storage[3];
    auto& y  = method.storage[4];

    // stages
    y = x;
    sys(t + dt, stages[3], y, k4);

    y = x + 0.5 * dt * k4;
    sys(t + dt / 2, stages[2], y, k3);

    y = x + 0.5 * dt * k3;
    sys(t + dt / 2, stages[1], y, k2);

    y = x + dt * k2;
    sys(t, stages[0], y, k1);

    // wrap up
    x = x + dt / 6 * (k1 + 2 * k2 + 2 * k3 + k4);
};
}