#pragma once
#include <vector>

namespace Flows {

////////////////////////////////////////////////////////////////
// Abstract class for monitors to be executed at runtime
template <typename X, typename T, typename S>
class AbstractMonitor {
public:
    // push snapshot into the monitor
    virtual void push_back(double t, const X& x) = 0;
    // return an object to iterate over times at which we have a snapshot
    virtual T&   times() = 0;
    // return an object to over the snapshots
    virtual S& samples() = 0;
};


////////////////////////////////////////////////////////////////
// A monitor that does nothing. This is used in various places
// where we do not pass a monitor, but for genericity, the code
// is written like there is one. We pass an instance of this type
// to make the code run without complaints.
template <typename X>
class NoOpMonitor : public AbstractMonitor<X, std::nullptr_t, std::nullptr_t> {
public:
    void push_back(double t, const X& x) override {}
    std::nullptr_t   times() { return nullptr; }
    std::nullptr_t samples() { return nullptr; }
};


////////////////////////////////////////////////////////////////
// RAM Storage Monitor
template <typename X>
class RAMMonitor : public AbstractMonitor<X,
                                          std::vector<double>,
                                          std::vector<X>> {
private:
    std::vector<X>      xs;
    std::vector<double> ts;
    
public:
    // constructor
    RAMMonitor(size_t sizehint = 0) {
        std::vector<X>      xs; xs.reserve(sizehint);
        std::vector<double> ts; ts.reserve(sizehint);
    }

    // store (t, x) into data structure
    void push_back(double t, const X& x) override {
        ts.push_back(t);
        X y = x;
        xs.push_back(y); // store a copy
    }

    // retrieve data
    std::vector<double>&   times() { return ts; }
    std::vector<X>&      samples() { return xs; }
};

}