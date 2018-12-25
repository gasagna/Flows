#pragma once
#include <vector>

namespace Flows {

////////////////////////////////////////////////////////////////
// A STORAGE THAT DOES NOTHING
// This is used in various places where we do not pass
// a monitor, but for genericity, the code is written like
// there is one.
struct NoOpStorage {
    template <typename X>
    void push_back(double t, const X& x) {}
    void times() {}
    void samples() {}
};

////////////////////////////////////////////////////////////////
// RAM STORAGE
template <typename X>
class RAMStorage {
private:
    std::vector<double> _ts;
    std::vector<X>      _xs;

public:
    void push_back(double t, const X& x) {
        _ts.push_back(t);
        _xs.push_back(x);
    }

    auto& times() { return _ts; }
    auto& samples() { return _xs; }
};

////////////////////////////////////////////////////////////////
// Functor that returns its argument untouched
struct Identity {
    template <typename X>
    const X& operator()(const X& x) {
        return x;
    }
};

////////////////////////////////////////////////////////////////
// Monitor
template <
    typename STORAGE = NoOpStorage,
    typename FUN     = Identity>
class Monitor {
private:
    STORAGE _storage;
    FUN     _fun;
    int     _count;
    int     _oneevery;

public:
    // constructor
    Monitor(STORAGE&& storage  = NoOpStorage(),
        FUN&&         fun      = Identity(),
        int           oneevery = 1)
        : _storage(std::forward<STORAGE>(storage))
        , _fun(std::forward<FUN>(fun))
        , _count(0)
        , _oneevery(oneevery) {}

    // store (t, _fun(x)) into storage
    template <typename X>
    void push_back(double t, const X& x) {
        if (_count % _oneevery == 0)
            _storage.push_back(t, _fun(x));
        _count += 1;
    }

    // retrieve data
    auto& times() { return _storage.times(); }
    auto& samples() { return _storage.samples(); }
};
}