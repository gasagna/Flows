#pragma once
#include <cstddef>
#include <tuple>

namespace Flows {

////////////////////////////////////////////////////////////////
// Dummy TYPE used to signal, e.g. that some component of the system
// is integrated only explictly. We set the output to zero
// as we are treating the system as fully explicit. This is useful
// when we integrate the quadrature equation fully explicitly.
// Otherwise we call the appropriate method
struct NoOpFunction {
    template <typename Z>
    void mul(Z& dzdt, const Z& z) { dzdt = 0; }

    template <typename Z, typename C>
    void ImcA_mul(Z& dzdt, const Z& z, const C c) { dzdt = z; }

    template <typename Z, typename C>
    void ImcA_div(Z& dzdt, const Z& z, const C c) { dzdt = z; }
};

////////////////////////////////////////////////////////////////
//
template <std::size_t N, typename EXT, typename IMT>
class System {
private:
    EXT _exTerm;
    IMT _imTerm;

public:
    ////////////////////////////////////////////////////////////////
    // CONSTRUCTORS

    // two arguments
    System(EXT&& exTerm, IMT&& imTerm)
        : _exTerm(std::forward<EXT>(exTerm))
        , _imTerm(std::forward<IMT>(imTerm)) {}

    // two tuples
    template <typename... T, typename... S>
    System(std::tuple<T&...>&& exTerm, std::tuple<S&...>&& imTerm)
        : _exTerm(exTerm)
        , _imTerm(imTerm) {
        // ensure we pass same number of arguments in both tuples
        static_assert(sizeof...(T) == sizeof...(S));
    }

    ////////////////////////////////////////////////////////////////
    // IMPLEMENT CALLABLE INTERFACE FOR THE EXPLICIT TERM
    // call with a simple argument
    template <typename Z>
    inline void operator()(double t, const Z& z, Z& dzdt) {
        static_assert(N == 1, "invalid number of inputs");
        _exTerm(t, z, dzdt);
    }

    // this is for the adjoint schemes, where x is the i-th stage
    template <typename Z>
    inline void operator()(double t, const Z& x, const Z& z, Z& dzdt) {
        static_assert(N == 1, "invalid number of inputs");
        _exTerm(t, x, z, dzdt);
    }

    // call with a pair, but check we actually have two functions
    template <typename ZA, typename ZB>
    void operator()(double t, const Pair<ZA, ZB>& z, Pair<ZA, ZB>& dzdt) {
        static_assert(N == 2, "invalid number of inputs");
        std::get<0>(_exTerm)(t, std::get<0>(z), std::get<0>(dzdt));
        std::get<1>(_exTerm)(t, std::get<0>(z), std::get<0>(dzdt), std::get<1>(z), std::get<1>(dzdt));
    }

    // call with a triplet, but check we actually have three functions
    template <typename ZA, typename ZB, typename ZC>
    void operator()(double t, const Triplet<ZA, ZB, ZC>& z, Triplet<ZA, ZB, ZC>& dzdt) {
        static_assert(N == 3, "invalid number of inputs");
        std::get<0>(_exTerm)(t, std::get<0>(z), std::get<0>(dzdt));
        std::get<1>(_exTerm)(t, std::get<0>(z), std::get<0>(dzdt), std::get<1>(z), std::get<1>(dzdt));
        std::get<2>(_exTerm)(t, std::get<0>(z), std::get<0>(dzdt), std::get<1>(z), std::get<1>(dzdt), std::get<2>(z), std::get<2>(dzdt));
    }

    ////////////////////////////////////////////////////////////////
    // IMPLICIT TERM. SEE ABOVE FOR THE GENERAL STRUCTURE
    template <typename Z>
    inline void mul(Z& dzdt, const Z& z) {
        _imTerm.mul(dzdt, z);
    }

    template <typename ZA, typename ZB>
    inline void mul(Pair<ZA, ZB>& dzdt, const Pair<ZA, ZB>& z) {
        std::get<0>(_imTerm).mul(std::get<0>(dzdt), std::get<0>(z));
        std::get<1>(_imTerm).mul(std::get<1>(dzdt), std::get<1>(z));
    }

    template <typename ZA, typename ZB, typename ZC>
    inline void mul(Triplet<ZA, ZB, ZC>& dzdt, const Triplet<ZA, ZB, ZC>& z) {
        std::get<0>(_imTerm).mul(std::get<0>(dzdt), std::get<0>(z));
        std::get<1>(_imTerm).mul(std::get<1>(dzdt), std::get<1>(z));
        std::get<2>(_imTerm).mul(std::get<2>(dzdt), std::get<2>(z));
    }

    ////////////////////////////////////////////////////////////////
#define _DEFINE_ImcA_xxx(_xxx)                                                              \
                                                                                            \
    template <typename Z, typename C>                                                       \
    inline void ImcA_##_xxx(Z& dzdt, const Z& z, C c) {                                     \
        _imTerm.ImcA_##_xxx(dzdt, z, c);                                                    \
    }                                                                                       \
                                                                                            \
    template <typename ZA, typename ZB, typename C>                                         \
    inline void ImcA_##_xxx(Pair<ZA, ZB>& dzdt, const Pair<ZA, ZB>& z, C c) {               \
        std::get<0>(_imTerm).ImcA_##_xxx(std::get<0>(dzdt), std::get<0>(z), c);             \
        std::get<1>(_imTerm).ImcA_##_xxx(std::get<1>(dzdt), std::get<1>(z), c);             \
    }                                                                                       \
                                                                                            \
    template <typename ZA, typename ZB, typename ZC, typename C>                            \
    inline void ImcA_##_xxx(Triplet<ZA, ZB, ZC>& dzdt, const Triplet<ZA, ZB, ZC>& z, C c) { \
        std::get<0>(_imTerm).ImcA_##_xxx(std::get<0>(dzdt), std::get<0>(z), c);             \
        std::get<1>(_imTerm).ImcA_##_xxx(std::get<1>(dzdt), std::get<1>(z), c);             \
        std::get<2>(_imTerm).ImcA_##_xxx(std::get<2>(dzdt), std::get<2>(z), c);             \
    }

    _DEFINE_ImcA_xxx(div)
        _DEFINE_ImcA_xxx(mul)

#undef _DEFINE_ImcA_xxx
};

////////////////////////////////////////////////////////////////
// TYPE DEDUCTION GUIDES FOR THE CONSTRUCTORS

template <typename... T, typename... S>
System(std::tuple<T&...>&&,
       std::tuple<S&...> &&)
    ->System<sizeof...(T), std::tuple<T&...>, std::tuple<S&...>>;

template <typename T, typename S>
System(T&&, S &&)->System<1, T, S>;
}