#pragma once
#include <array>

namespace Flows {

////////////////////////////////////////////////////////////////
// THE BASE CLASS OF ALL STAGE CACHES
template <typename X>
class AbstractStageCache {
public:
    // this prepares storage space for a new step
    virtual void setup_step(double t, double dt) = 0;

    // push a stage vector into the cache
    virtual void push_back(const X& x) = 0;

    // end-of-step function
    virtual void close_step() = 0;
};

////////////////////////////////////////////////////////////////
/// This subclass does nothing and is used as a default
template <typename X>
struct NoOpStageCache : public AbstractStageCache<X> {
    NoOpStageCache() {}
    inline void setup_step(double t, double dt) override {}
    inline void push_back(const X& x) override {}
    inline void close_step() override {}
};

}