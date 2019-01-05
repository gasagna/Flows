#pragma once
#include "../stagecache.hpp"
#include "../system.hpp"
#include "../tableaux.hpp"
#include "generic.hpp"

namespace Flows {

//////////////////////////////////////////////////////////////////////////////////////////
// define all 3R2R methods with a macro
#define _DEFINE_CB3R2R_METHOD(_NAME, _NSTAGES, _TABLEAU)                         \
                                                                                 \
    template <typename Y, bool ISADJOINT = false>                                \
    struct _NAME : public AbstractMethod<Y, 4, ISADJOINT> {                      \
        _NAME(const Y& x)                                                        \
            : AbstractMethod<Y, 4, ISADJOINT>(x) {}                              \
    };                                                                           \
                                                                                 \
    template <typename Y, typename X, typename SYSTEM, typename STAGECACHE>      \
    void step(_NAME<Y, false>& method,                                           \
              SYSTEM&          sys,                                              \
              double           t,                                                \
              double           dt,                                               \
              X&               x,                                                \
              STAGECACHE&&     c) {                                                  \
                                                                                 \
        auto& y  = method.storage[0];                                            \
        auto& z  = method.storage[1];                                            \
        auto& w  = method.storage[2];                                            \
        auto& Ay = method.storage[3];                                            \
                                                                                 \
        static const IMEXTableau<_NSTAGES> tab = _TABLEAU;                       \
                                                                                 \
        c.setup_step(t, dt);                                                     \
                                                                                 \
        for (int k = 0; k != _NSTAGES; k++) {                                    \
            if (k == 0) {                                                        \
                y = x;                                                           \
            } else {                                                             \
                auto c1 = (tab('I', 'a', k, k - 1) - tab('I', 'b', k - 1)) * dt; \
                auto c2 = (tab('E', 'a', k, k - 1) - tab('E', 'b', k - 1)) * dt; \
                y       = x + c1 * z + c2 * y;                                   \
            }                                                                    \
            sys.mul(Ay, y);                                                      \
            sys.mul(z, Ay, tab('I', 'a', k, k) * dt);                            \
            w = y + (tab('I', 'a', k, k) * dt) * z;                              \
            c.push_back(w);                                                      \
            sys(t + tab('E', 'c', k) * dt, w, y);                                \
            x = x + (tab('I', 'b', k) * dt) * z + (tab('E', 'b', k) * dt) * y;   \
        }                                                                        \
        c.close_step();                                                          \
    }                                                                            \
                                                                                 \
    template <typename Y, typename X, typename SYSTEM, typename STAGES>          \
    void step(_NAME<Y, true>& method,                                            \
              SYSTEM&         sys,                                               \
              double          t,                                                 \
              double          dt,                                                \
              X&              x,                                                 \
              STAGES&&        stages) {                                                 \
                                                                                 \
        auto& y = method.storage[0];                                             \
        auto& z = method.storage[1];                                             \
        auto& w = method.storage[2];                                             \
                                                                                 \
        static const IMEXTableau<_NSTAGES> tab = _TABLEAU;                       \
                                                                                 \
        y = 0;                                                                   \
        z = 0;                                                                   \
        w = 0;                                                                   \
                                                                                 \
        for (int k = _NSTAGES - 1; k >= 0; k--) {                                \
            z = z + tab('I', 'b', k) * dt * x;                                   \
            y = y + tab('E', 'b', k) * dt * x;                                   \
            sys(t + tab('E', 'c', k) * dt, stages[k], y, w);                     \
            z = z + tab('I', 'a', k, k) * dt * w;                                \
            y = w;                                                               \
            sys.mul(w, z, tab('I', 'a', k, k) * dt);                             \
            sys.mul(z, w);                                                       \
            y = y + z;                                                           \
            x = x + y;                                                           \
            if (k > 0) {                                                         \
                z = (tab('I', 'a', k, k - 1) - tab('I', 'b', k - 1)) * dt * y;   \
                y = (tab('E', 'a', k, k - 1) - tab('E', 'b', k - 1)) * dt * y;   \
            }                                                                    \
        }                                                                        \
    }

_DEFINE_CB3R2R_METHOD(CB3R2R_3E, 4, CB3e)
_DEFINE_CB3R2R_METHOD(CB3R2R_2, 3, CB2)

#undef _DEFINE_CB3R2R_METHOD
}