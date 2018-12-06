#pragma once
#include <array>

namespace Flows {

////////////////////////////////////////////////////////////////
// Abstract class for integration methods
template <typename X, std::size_t N>
struct AbstractMethod {
    std::array<X, N> storage;
    AbstractMethod(const X& x) : storage ({x}) {}
};

}