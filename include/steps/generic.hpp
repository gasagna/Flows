#pragma once
#include <vector>

namespace Flows {

////////////////////////////////////////////////////////////////
// Abstract class for integration methods
template <typename X, std::size_t N, bool ISADJOINT = false>
struct AbstractMethod {
    // we use a vector rather than an array because the latter
    // forces you to have a default constructor, which you might
    // not have in a general case.
    std::vector<X> storage;
    AbstractMethod(const X& x)
        : storage(N, x) {}
};

// trait used to determine whether we are integrating an adjoint problem
template <typename T>
struct isAdjoint : std::false_type {};

template <typename X, std::size_t N>
struct isAdjoint<AbstractMethod<X, N, true>> : std::true_type {};

}