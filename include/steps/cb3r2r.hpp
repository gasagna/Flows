#pragma once
#include "generic.hpp"
#include "../system.hpp"
#include "../tableaux.hpp"
#include "../stagecache.hpp"

namespace Flows {

//////////////////////////////////////////////////////////////////////////////////////////
// define all 3R2R methods with a macro                                                  
#define _DEFINE_CB3R2R_METHOD(_NAME, _NSTAGES, _TABLEAU)                                 \
                                                                                         \
    template <typename X>                                                                \
    struct _CB3R2R : public AbstractMethod<X, 3> {                                       \
        _CB3R2R(const X& x) : AbstractMethod<X, 3>(x) {}                                 \
    };                                                                                   \
                                                                                         \
    template <typename X, typename SYSTEM, typename STAGECACHE>                          \
    void step(const _CB3R2R<X>& method,                                                  \
              const SYSTEM& sys,                                                         \
              double t,                                                                  \
              double dt,                                                                 \
              X& x,                                                                      \
              STAGECACHE& c) {                                                           \
                                                                                         \
        auto& y   = method.storage[0];                                                   \
        auto& z   = method.storage[1];                                                   \
        auto& w   = method.storage[2];                                                   \
                                                                                         \
        static const IMEXTableau<_NSTAGES> tab = _TABLEAU;                               \
                                                                                         \
        c.setup(t, dt);                                                                  \
                                                                                         \
        for (int k = 0; k != _NSTAGES; k++) {                                            \
            if (k == 0) {                                                                \
                y = x;                                                                   \
            } else {                                                                     \
                y = x + (tab('I', 'a', k, k-1) - tab('I', 'b', k-1))*dt*z +              \
                        (tab('E', 'a', k, k-1) - tab('E', 'b', k-1))*dt*y;               \
            }                                                                            \
            sys.mul(z, y);                                                               \
            sys.mul(z, z, tab('I', 'a', k, k)*dt);                                       \
            w = y + tab('I', 'a', k, k)*dt*z;                                            \
            c.push(w)                                                                    \
            sys(t + tab('E', 'c', k)*dt, w, y);                                          \
            x = x + tab('I', 'b', k)*dt*z + tab('E', 'b', k)*dt*y;                       \
        }                                                                                \
    }                                                                                    \

_DEFINE_CB3R2R_METHOD(CB3R2R_3E, 4, CB3e)
_DEFINE_CB3R2R_METHOD(CB3R2R_2,  3, CB2)

#undef _DEFINE_CB3R2R_METHOD

}