#pragma once

#include "coupled.hpp"

namespace Flows {

////////////////////////////////////////////////////////////////
/// Base class of Monitors
template <typename X>
struct AbstractMonitor {
    virtual void push_back(double t, const X& x) = 0;
};

////////////////////////////////////////////////////////////////
// A monitor that does nothing
// This is used in various places where we do not pass
// a monitor, but for genericity, the code is written like
// there is one. The method psh_back is a noop, and should be
// compiled away by the compiler (zero cost abstraction).
template <typename X>
struct NoOpMonitor : public AbstractMonitor<X> {
    void push_back(double t, const X& x) override {}
};

////////////////////////////////////////////////////////////////
// Functor that returns its argument untouched
struct Identity {
    template <typename X>
    constexpr const X& operator()(X&& x) {
        return std::forward<X>(x);
    }
};

// when we pass a Pair or Triplet that contains
// references, we make a copy of the data and return it.
// There should not be any further copies down the line
// because
struct Copy {
    template <typename A, typename B>
    Pair<A, B> operator()(const Pair<A&, B&>& x) {
        return couple(std::get<0>(x), std::get<1>(x));
    }

    template <typename A, typename B, typename C>
    Triplet<A, B, C> operator()(const Triplet<A&, B&, C&>& x) {
        return couple(std::get<0>(x), std::get<1>(x), std::get<2>(x));
    }
};

////////////////////////////////////////////////////////////////
// Monitor
template <typename X, typename STORAGE, typename FUN>
class Monitor : public AbstractMonitor<X> {
private:
    STORAGE _storage;
    FUN     _fun;
    int     _count;
    int     _oneevery;

public:
    // constructor
    Monitor(const X& x,
        STORAGE&&    storage,
        FUN&&        fun,
        int          oneevery = 1)
        : _storage(std::forward<STORAGE>(storage))
        , _fun(std::forward<FUN>(fun))
        , _count(0)
        , _oneevery(oneevery) {}

    // store (t, _fun(x)) into storage
    void push_back(double t, const X& x) override {
        if (_count % _oneevery == 0)
            _storage.push_back(t, _fun(x));
        _count += 1;
    }

    // retrieve data
    auto& times() { return _storage.times(); }
    auto& samples() { return _storage.samples(); }
};
}