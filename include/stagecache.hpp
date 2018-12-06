#pragma once
#include <array>

namespace Flows {

////////////////////////////////////////////////////////////////
// THE BASE CLASS OF ALL STAGE CACHES
template<typename X, std::size_t N>
class AbstractStageCache {
public:
    // this prepares storage space for a new step
    virtual void setup(double t, double dt) = 0;

    // push the ith stage vector into the cache
    template<std::size_t i> void push_back(X& x);
};

////////////////////////////////////////////////////////////////
/// This subclass does nothing and is used as a default
template<typename X, std::size_t N>
class NoOpStageCache : public AbstractStageCache<X, N> {
public:
    NoOpStageCache() {}
    void setup(double t, double dt) override {}
    template <std::size_t n> void push_back(const X& x) {}
};

}