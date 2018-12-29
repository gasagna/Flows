#pragma once

#include <type_traits>

#include "coupled.hpp"
#include "monitor.hpp"
#include "stagecache.hpp"
#include "stepping.hpp"
#include "timerange.hpp"

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
    STAGECACHE&&                cache) {

    if (t_from == t_to)
        throw std::invalid_argument("time span endpoints must differ");

    // march in time
    for (auto [t, dt_loc] : TimeRange(t_from, t_to, stepping.dt)) {

        // push state to monitor
        m.push_back(t, x);

        // make a step
        step(method, system, t, dt_loc, x, std::forward<STAGECACHE>(cache));
    }

    // push final state to monitor before exiting
    m.push_back(t_to, x);

    return x;
}

////////////////////////////////////////////////////////////////
// map state from t_from to t_to using a stage cache
template <
    typename X,
    typename SYSTEM,
    typename METHOD, // number of storage vectors
    typename Y, // data type (stripped of references for Pairs and Triplets)
    typename STAGECACHE> // number of stages
X&
_propagate(TimeStepFromStageCache& stepping,
    SYSTEM&                        system,
    METHOD&                        method,
    X&                             x,
    STAGECACHE&&                   cache) {

    static_assert(isAdjoint<METHOD>::value, 
        "can't integrate non adjoint system using stage cache");

    for (auto [t, dt, stages] : reverse(cache))
        step(method, system, t, dt, x, stages);

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

    // just integrate the equations
    template <typename X, typename T1, typename T2>
    X& operator()(X& x, T1 t_from, T2 t_to) {
        // the stage cache will receive copycouples, not refcouples
        // so we need to create a type that matches the type of input
        using Y = remove_refs_from_coupled_t<X>;
        return _propagate(_stepping,
            _system,
            _method,
            x,
            double(t_from),
            double(t_to),
            NoOpMonitor<X>(),
            NoOpStageCache<Y>());
    }

    // integrate the equations and fill a monitor
    template <typename X, typename T1, typename T2>
    X& operator()(X& x, T1 t_from, T2 t_to, AbstractMonitor<X>& m) {
        using Y = remove_refs_from_coupled_t<X>;
        return _propagate(_stepping,
            _system,
            _method,
            x,
            double(t_from),
            double(t_to),
            m,
            NoOpStageCache<Y>());
    }

    // fill the stage cache from t_from to t_to
    template <typename X, typename T1, typename T2, typename Y, std::size_t N>
    X& operator()(X& x, T1 t_from, T2 t_to, AbstractStageCache<Y, N>& c) {
        static_assert(is_ref_compatible_v<X, Y>,
            "incompatible cache and input types");
        return _propagate(_stepping,
            _system,
            _method,
            x,
            double(t_from),
            double(t_to),
            NoOpMonitor<X>(),
            c);
    }

    // integrate based on a stage cache only, i.e. not filling the cache
    // but using the stages stored for the forward/backward integration.
    template <typename X, typename Y, std::size_t N>
    X& operator()(X& x, AbstractStageCache<Y, N>& c) {
        static_assert(is_ref_compatible_v<X, Y>,
            "incompatible cache and input types");
        return _propagate(_stepping,
            _system,
            _method,
            x,
            c);
    }
};
}