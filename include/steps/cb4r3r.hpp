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

    template <size_t N>
    void step(X& x, double t, double dt, IMEXTableau<N>& tab, RK_3R2R) noexcept {
        // make aliases
        X& y = this->storage[0];
        X& z = this->storage[1];
        X& w = this->storage[2];

        // tableau stages
        for (int k = 0; k != N; k++) {
            if (k == 0) {
                y = x;
            } else {
                y = x + (tab('I', 'a', k, k-1) - tab('I', 'b', k-1))*dt*z +
                        (tab('E', 'a', k, k-1) - tab('E', 'b', k-1))*dt*y;
            }
            this->rhs.imterm(z, y);
            this->rhs.imterm(tab('I', 'a', k, k)*dt, z, z);
            w = y + tab('I', 'a', k, k)*dt*z;
            this->rhs.exterm(t + tab('E', 'c', k)*dt, w, y);
            x = x + tab('I', 'b', k)*dt*z + tab('E', 'b', k)*dt*y;
        }
    }

    // specialisation of step for RK_4R3R
    template <size_t N>
    void step(X &x, double t, double dt, IMEXTableau<N> &tab, RK_4R3R) noexcept {
        // make aliases
        X& y  = this->storage[0];
        X& zi = this->storage[1];
        X& ze = this->storage[2];
        X& w  = this->storage[3];

        // tableau stages
        for (int k = 0; k != N; k++) {
            if (k == 0) {
                 y = x;
                zi = x;
                ze = x;
            } else {
                ze = y + tab('E', 'a', k, k-1)*dt*ze;
                if (k < N-1) {
                    y = x + (tab('I', 'a', k+1, k-1) - tab('I', 'b', k-1))*dt*zi +
                            (tab('E', 'a', k+1, k-1) - tab('I', 'b', k-1))/tab('E', 'a', k, k-1)*(ze - y);
                }
                ze = ze + tab('I', 'a', k, k-1)*dt*zi;
            }
            this->rhs.imterm(w, ze);
            this->rhs.imterm(tab('I', 'a', k, k)*dt, w, zi);
            w = ze + tab('I', 'a', k, k)*dt*zi;
            this->rhs.exterm(t + tab('E', 'c', k)*dt, w, ze);
            x = x + tab('I', 'b', k)*dt*zi + tab('E', 'b', k)*dt*ze;
        }
    }


    // second order method
Tableau<3> _CB2I = {{0.0 / 1.0, 0.0 / 1.0, 0.0 / 1.0,
                     0.0 / 1.0, 2.0 / 5.0, 0.0 / 1.0,
                     0.0 / 1.0, 5.0 / 6.0, 1.0 / 6.0},
                    {0.0 / 1.0, 5.0 / 6.0, 1.0 / 6.0},
                    {0.0 / 1.0, 2.0 / 5.0, 1.0 / 1.0}};
Tableau<3> _CB2E = {{0.0 / 1.0, 0.0 / 1.0, 0.0 / 1.0,
                     2.0 / 5.0, 0.0 / 1.0, 0.0 / 1.0,
                     0.0 / 1.0, 1.0 / 1.0, 0.0 / 1.0},
                    {0.0 / 1.0, 5.0 / 6.0, 1.0 / 6.0},
                    {0.0 / 1.0, 2.0 / 5.0, 1.0 / 1.0}};
IMEXTableau<3> CB2{_CB2I, _CB2E};

// third order method
Tableau<4> _CB3eI = {{0.0 / 1.0, 0.0 / 1.0, 0.0 / 1.0, 0.0 / 1.0,
                      0.0 / 1.0, 1.0 / 3.0, 0.0 / 1.0, 0.0 / 1.0,
                      0.0 / 1.0, 1.0 / 2.0, 1.0 / 2.0, 0.0 / 1.0,
                      0.0 / 1.0, 3.0 / 4.0, -1.0 / 4.0, 1.0 / 2.0},
                     {0.0 / 1.0, 3.0 / 4.0, -1.0 / 4.0, 1.0 / 2.0},
                     {0.0 / 1.0, 1.0 / 3.0, 1.0 / 1.0, 1.0 / 1.0}};
Tableau<4> _CB3eE = {{0.0 / 1.0, 0.0 / 1.0, 0.0 / 1.0, 0.0 / 1.0,
                      1.0 / 3.0, 0.0 / 1.0, 0.0 / 1.0, 0.0 / 1.0,
                      0.0 / 1.0, 1.0 / 1.0, 0.0 / 1.0, 0.0 / 1.0,
                      0.0 / 1.0, 3.0 / 4.0, 1.0 / 4.0, 0.0 / 1.0},
                     {0.0 / 1.0, 3.0 / 4.0, -1.0 / 4.0, 1.0 / 2.0},
                     {0.0 / 1.0, 1.0 / 3.0, 1.0 / 1.0, 1.0 / 1.0}};
IMEXTableau<4> CB3e{_CB3eI, _CB3eE};