#pragma once
#include <type_traits>
#include "timerange.hpp"
#include "monitor.hpp"
#include "stagecache.hpp"
#include "stepping.hpp"

namespace Flows {

template <
    typename S,
    typename M,
    typename T
> 
class Flow {
private:
    const S&      sys;
    const M&   method;
    const T& stepping;

    ////////////////////////////////////////////////////////////////
    // map state from t_from to t_to using constant time stepping
    template <
        typename X,
        typename MONITOR,
        typename SCACHE,
        std::enable_if_t<std::is_same<T, TimeStepConstant>::value>
    >
    X& _propagate(const T& stepping,
                        X& x,
                    double t_from,
                    double t_to,
                    double dt,
                  MONITOR& m,
                   SCACHE& c) {
        
        if (t_from == t_to)
            throw std::invalid_argument("time span endpoints must differ");

        // march in time
        for (auto [t, dt_loc] : TimeRange(t_from, t_to, stepping.dt)) {

            // push state to monitor
            // m.push_back(t, x);

            // make a step
            step(method, t, dt_loc, x, c);
        }

        // push final state to monitor before exiting
        m.push_back(t_to, x);

        return x;
    }

public:
    // constructor
    Flow(const S& _sys, const M& _method, const T& _stepping)
        : sys      (_sys    )
        , method   (_method )
        , stepping (_stepping) {
            // check input arguments have consistent type
            // https://stackoverflow.com/a/22934960/252544
            static_assert( std::is_base_of_v<System, S> );
            static_assert( std::is_base_of_v<AbstractMethod, M> );
            static_assert( std::is_base_of_v<AbstractTimeStepping, T> );
        }

    ////////////////////////////////////////////////////////////////
    // Flow objects are callable
    template <typename X, typename MONITOR, typename SCACHE>
    X& operator () (X& x, double t_from, double t_to, MONITOR& m, SCACHE& c) {
        _propagate(stepping, x, t_from, t_to, m, c);
    }

    template <typename X>
    X& operator () (X& x, double t_from, double t_to) {
        _propagate(stepping, x, t_from, t_to, NoOpMonitor<X>(), NoOpStageCache<X, 0>());
    }

    template <
        typename X,
        typename MONITOR,
        typename std::enable_if_t<std::is_base_of_v<MONITOR, AbstractMonitor>>
    >
    X& operator () (X& x, double t_from, double t_to, MONITOR& m) {
        _propagate(stepping, x, t_from, t_to, m, NoOpStageCache<X, 0>());
    }

    template <
        typename X, 
        typename SCACHE,
        typename std::enable_if_t<std::is_base_of_v<SCACHE, AbstractStageCache>>
    >
    X& operator () (X& x, double t_from, double t_to, SCACHE& c) {
        _propagate(stepping, x, t_from, t_to, NoOpMonitor<X>(), c);
    }

};
}