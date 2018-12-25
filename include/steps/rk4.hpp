#pragma once
#include "generic.hpp"
#include "stagecache.hpp"
#include "system.hpp"
#include <array>

namespace Flows {

////////////////////////////////////////////////////////////////
// Classical fourth order Runge-Kutta method
template <typename X>
struct RK4 : public AbstractMethod<X, 5> {
    RK4(const X& x)
        : AbstractMethod<X, 5>(x) {}
};

template <typename X, typename SYSTEM, typename STAGECACHE>
void step(RK4<X>& method,
    SYSTEM&       sys,
    double        t,
    double        dt,
    X&            x,
    STAGECACHE&&  c) {

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
}