#pragma once
#include <tuple>

namespace Flows {

////////////////////////////////////////////////////////////////
// trick to construct a tuple of length sizeof...(args) with object of type T
template <typename T, typename... args>
using _splat = T;

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
    void mul(Z& dzdt, const Z& z, const C c) { dzdt = z; }
};

////////////////////////////////////////////////////////////////
//
template <typename EXT, typename IMT>
class System {
private:
    EXT& _exTerm;
    IMT& _imTerm;

public:
    ////////////////////////////////////////////////////////////////
    // CONSTRUCTORS
    System(EXT& exTerm, IMT& imTerm)
        : _exTerm(exTerm)
        , _imTerm(imTerm) {}

    // : System(std::make_tuple(_exTerm), std::make_tuple(_imTerm)) {}

    // template <typename... T>
    // System(const std::tuple<T...>& _exTerm)
    //     : System(_exTerm, std::tuple<_splat<NoOpFunction, T>...>{}) {}

    // template <typename... T, typename... S>
    // System(const std::tuple<T...>& _exTerm, const std::tuple<S...>& _imTerm)
    //     : exTerm(_exTerm)
    //     , imTerm(_imTerm) {
    //     // ensure we pass same number of arguments in both tuples
    //     static_assert(sizeof...(T) == sizeof...(S));
    // }

    ////////////////////////////////////////////////////////////////
    // IMPLEMENT CALLABLE INTERFACE FOR THE EXPLICIT TERM
    // call with a simple argument
    template <typename Z>
    inline void operator()(double t, const Z& z, Z& dzdt) {
        // static_assert(N == 1, "invalid number of inputs");
        _exTerm(t, z, dzdt);
    }

    // // call with a pair, but check we actually have two functions
    // template <typename... Z>
    // void operator()(double t,
    //     const Pair<Z...>&  z,
    //     Pair<Z...>&        dzdt) {
    //     // static_assert(N == 2; "invalid number of inputs");
    //     std::get<0>(exTerm)(t, get<0>(z), get<0>(dzdt));
    //     std::get<1>(exTerm)(t, get<0>(z), get<0>(dzdt), get<1>(z), get<1>(dzdt));
    // }

    // // call with a triplet,  but check we actually have two functions
    // template <typename... Z>
    // void operator()(double   t,
    //     const Triplet<Z...>& z,
    //     Triplet<Z...>&       dzdt) {
    //     static_assert(N == 3; "invalid number of inputs");
    //     std::get<0>(exTerm)(t, get<0>(z), get<0>(dzdt));
    //     std::get<1>(exTerm)(t, get<0>(z), get<0>(dzdt),
    //         get<1>(z), get<1>(dzdt));
    //     std::get<2>(exTerm)(t, get<0>(z), get<0>(dzdt),
    //         get<1>(z), get<1>(dzdt),
    //         get<2>(z), get<2>(dzdt));
    // }

    ////////////////////////////////////////////////////////////////
    // IMPLICIT TERM. SEE ABOVE FOR THE GENERAL STRUCTURE
    template <typename Z>
    inline void mul(Z& dzdt, const Z& z) {
        _imTerm.mul(dzdt, z);
    }

    template <typename Z, typename C>
    inline void mul(Z& dzdt, const Z& z, const C c) {
        _imTerm.mul(dzdt, z, c);
    }

    // template <typename... Z>
    // void mul(const Pair<Z...>& z, Pair<Z...>& dzdt) {
    //     _mul_nth(std::get<0>(imTerm), get<0>(z)), get<0>(dzdt));
    //     _mul_nth(std::get<1>(imTerm), get<1>(z)), get<1>(dzdt));
    // }

    // template <typename... Z>
    // void mul(const Triplet<Z...>& z, Pair<Z...>& dzdt) {
    //     _mul_nth(std::get<0>(imTerm), get<0>(z)), get<0>(dzdt));
    //     _mul_nth(std::get<1>(imTerm), get<1>(z)), get<1>(dzdt));
    //     _mul_nth(std::get<2>(imTerm), get<2>(z)), get<2>(dzdt));
    // }
};

////////////////////////////////////////////////////////////////
// TYPE DEDUCTION GUIDES FOR THE CONSTRUCTORS
// template <typename... T>
// System(std::tuple<T...>)->System<sizeof...(T), std::tuple<T...>, std::tuple<_splat<NoOpFunction, T>...>>;

// template <typename... T, typename... S>
// System(std::tuple<T...>,
//     std::tuple<S...>)
//     ->System<sizeof...(T), std::tuple<T...>,
//         std::tuple<S...>>;

// template <typename T>
// System(T)->System<1, std::tuple<T>, std::tuple<NoOpFunction>>;

template <typename T>
System(T)->System<T, NoOpFunction>;

// template <typename T, typename S>
// System(T, S)->System<1, std::tuple<T>, std::tuple<S>>;
}