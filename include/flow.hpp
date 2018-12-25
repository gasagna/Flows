#pragma once
#include "monitor.hpp"
#include "stagecache.hpp"
#include "stepping.hpp"
#include "timerange.hpp"
#include <type_traits>

namespace Flows {

////////////////////////////////////////////////////////////////
// PROPAGATION FUNCTIONS

////////////////////////////////////////////////////////////////
// map state from t_from to t_to using constant time stepping
template <
    typename X,
    typename SYSTEM,
    typename METHOD,
    typename MONITOR,
    typename STAGECACHE>
X& _propagate(TimeStepConstant& stepping,
    SYSTEM&                     system,
    METHOD&                     method,
    X&                          x,
    double                      t_from,
    double                      t_to,
    MONITOR&&                   m,
    STAGECACHE&&                c) {

    if (t_from == t_to)
        throw std::invalid_argument("time span endpoints must differ");

    // march in time
    for (auto [t, dt_loc] : TimeRange(t_from, t_to, stepping.dt)) {

        // push state to monitor
        m.push_back(t, x);

        // make a step
        step(method, system, t, dt_loc, x, std::forward<STAGECACHE>(c));
    }

    // push final state to monitor before exiting
    m.push_back(t_to, x);

    return x;
}

////////////////////////////////////////////////////////////////
// THE FLOW OBJECT
template <
    typename SYSTEM,
    typename METHOD,
    typename STEPPING>
class Flow {
private:
    SYSTEM&   _system;
    METHOD&   _method;
    STEPPING& _stepping;

public:
    // constructor
    Flow(SYSTEM& system, METHOD& method, STEPPING& stepping)
        : _system(system)
        , _method(method)
        , _stepping(stepping) {
        // check input arguments have consistent type
        // https://stackoverflow.com/a/22934960/252544
        // static_assert(std::is_base_of_v<System, S>);
        // static_assert(std::is_base_of_v<AbstractMethod, M>);
        // static_assert(std::is_base_of_v<AbstractTimeStepping, T>);
    }

    ////////////////////////////////////////////////////////////////
    // Flow objects are callable
    template <
        typename X,
        typename T1,
        typename T2,
        typename STORAGE,
        typename FUN,
        typename STAGECACHE>
    X& operator()(X& x, T1 t_from, T2 t_to, Monitor<STORAGE, FUN>&& m, STAGECACHE&& c) {
        return _propagate(_stepping,
            _system,
            _method,
            x,
            double(t_from),
            double(t_to),
            std::forward<Monitor<STORAGE, FUN>>(m),
            std::forward<STAGECACHE>(c));
    }

    template <typename X, typename T1, typename T2>
    X& operator()(X& x, T1 t_from, T2 t_to) {
        return _propagate(_stepping,
            _system,
            _method,
            x,
            double(t_from),
            double(t_to),
            Monitor(),
            NoOpStageCache<X>());
    }

    template <typename X, typename T1, typename T2, typename STORAGE, typename FUN>
    X& operator()(X& x, T1 t_from, T2 t_to, Monitor<STORAGE, FUN>& m) {
        return _propagate(_stepping,
            _system,
            _method,
            x,
            double(t_from),
            double(t_to),
            m,
            NoOpStageCache<X>());
    }

    template <typename X, typename T1, typename T2, typename SCACHE>
    X& operator()(X& x, T1 t_from, T2 t_to, AbstractStageCache<X>& c) {
        return _propagate(_stepping,
            _system,
            _method,
            x,
            double(t_from),
            double(t_to),
            Monitor(),
            c);
    }
};
}