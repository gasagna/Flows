#pragma once

#include <cstddef>
#include <vector>

namespace Flows {

////////////////////////////////////////////////////////////////
/// Base class of stage caches
template <typename X, std::size_t N>
struct AbstractStageCache {
    virtual void setup_step(double t, double dt) = 0;
    virtual void push_back(const X& x)           = 0;
    virtual void close_step()                    = 0;
};

////////////////////////////////////////////////////////////////
/// This subclass does nothing and is used as a default
template <typename X>
struct NoOpStageCache : public AbstractStageCache<X, 0> {
    inline void setup_step(double t, double dt) override {}
    inline void push_back(const X& x) override {}
    inline void close_step() override {}
};

////////////////////////////////////////////////////////////////
// Provides a const view of N elements over a container. The
// view is indexable using the subscript operator, but no bound
// checking is performed. This is used because we store all stages
// in a linear fashion, e.g. in a std::vector and then need to
// provide an indexable structure for every call to the appropriate
// step function.
template <typename CONTAINER, std::size_t N>
class View {
private:
    const CONTAINER& _cont;
    std::size_t      _at;

public:
    View(const CONTAINER& cont, std::size_t at)
        : _cont(cont)
        , _at(at) {}

    const auto& operator[](std::size_t i) const {
        return _cont[_at + i];
    }
};

////////////////////////////////////////////////////////////////
// An iterator over the stages. This is used to provide
// iteration facilities that allow doing
//
//    for (auto [t, dt, stages] : cache)
//
template <typename VEC_T, typename VEC_X, std::size_t N>
class StageIterator {
private:
    // These are generic containers
    const VEC_T& _ts;
    const VEC_T& _dts;
    const VEC_X& _xs;
    size_t       _i;

public:
    /* Constructor */
    StageIterator(const VEC_T& ts, const VEC_T& dts, const VEC_X& xs, size_t i)
        : _ts(ts)
        , _dts(dts)
        , _xs(xs)
        , _i(i) {}

    /* Dereferencing */
    auto operator*() {
        return std::make_tuple(_ts[_i], _dts[_i], View<VEC_X, N>(_xs, _i * N));
    }

    /* Incrementing/Decrementing */
    auto& operator++() {
        _i++;
        return *this;
    }
    auto& operator--() {
        _i--;
        return *this;
    }

    /* Comparison */
    bool operator!=(const StageIterator<VEC_T, VEC_X, N>& other) const {
        return other._i != _i;
    }
};

////////////////////////////////////////////////////////////////
// Allow reverse iteration for the backward iteration
template <typename VEC>
class reverse {
private:
    const VEC& _vec;

public:
    reverse(const VEC& vec)
        : _vec(vec) {}

    auto begin() const {
        return std::make_reverse_iterator(std::end(_vec));
    }
    auto end() const {
        return std::make_reverse_iterator(std::begin(_vec));
    }
};

////////////////////////////////////////////////////////////////
// RAM STAGE CACHE
template <typename X, std::size_t N>
class RAMStageCache : public AbstractStageCache<X, N> {
private:
    std::vector<X>      _xs;
    std::vector<double> _ts;
    std::vector<double> _dts;

public:
    // this prepares storage space for a new step
    void setup_step(double t, double dt) override {
        _ts.push_back(t);
        _dts.push_back(dt);
    }

    // push a stage vector into the cache
    void push_back(const X& x) override {
        // This makes a copy of x, so it is important to make
        // sure that x does not contain any references member
        // variables. For coupled integration this is ensured
        // by the fact that, in the appropriate step function,
        // see e.g. RK4, we push Pair<A, B> object in the
        // method storage and not Pair<A&, B&> that is updated
        // at every step
        _xs.push_back(x);
    }

    // end-of-step function
    void close_step() override{ /* does nothing */ };

    // indexing (mainly for testing code)
    auto operator[](std::size_t i) {
        return std::make_tuple(_ts[i], _dts[i], View<std::vector<X>, N>(_xs, i * N));
    }

    // iteration support
    auto begin() const {
        return StageIterator<std::vector<double>, std::vector<X>, N>(_ts, _dts, _xs, 0);
    }
    auto end() const {
        return StageIterator<std::vector<double>, std::vector<X>, N>(_ts, _dts, _xs, _ts.size());
    }
};
}