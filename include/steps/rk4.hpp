#pragma once
#include <array>
#include "generic.hpp"
#include "system.hpp"
#include "stagecache.hpp"

namespace Flows {

////////////////////////////////////////////////////////////////
// Classical fourth order Runge-Kutta method
template <typename X>
struct RK4 : public AbstractMethod<X, 5> {
    RK4(const X& x) : AbstractMethod<X, 5>(x) {}
};

template <typename X, typename SYSTEM, typename STAGECACHE>
void step(const RK4<X>& method,
          const SYSTEM& sys,
          double t,
          double dt,
          X& x,
          STAGECACHE& c) {

    // aliases
    auto k1 = method.storage[0];
    auto k2 = method.storage[1];
    auto k3 = method.storage[2];
    auto k4 = method.storage[3];
    auto  y = method.storage[4];

    // prepare cache for new step
    c.setup(t, dt);

    // stages
    y = x          ; sys(t       , y, k1); c.push<0>(y);
    y = x + dt*k1/2; sys(t + dt/2, y, k2); c.push<1>(y);
    y = x + dt*k2/2; sys(t + dt/2, y, k3); c.push<2>(y);
    y = x + dt*k3  ; sys(t + dt  , y, k4); c.push<3>(y);

    // wrap up
    x = x + dt/6 * (k1 + 2*k2 + 2*k3 + k4);
};

}