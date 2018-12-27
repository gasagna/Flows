#pragma once
#include <iostream>
#include <type_traits>
#include <utility>

namespace Flows {

//////////////////////////////////////////////////////////////////////////////////////////
// Forward declarations

template <typename A, typename B>
struct Pair;

template <typename A, typename B, typename C>
struct Triplet;

//////////////////////////////////////////////////////////////////////////////////////////
// All objects will inherit from this, in order ot avoid polluting the namespace
// and avoiding catch all situations that you end up with using expression templates.
// We will probably never have Pair and Triplet together, so we only define
// one object.

template <typename E>
struct CoupledExpr {
    operator const E&() const {
        return *static_cast<const E*>(this);
    }
};

//////////////////////////////////////////////////////////////////////////////////////////
// Argument getters. These behave similarly to std::get for tuples.
namespace std {
    template <std::size_t N, typename A, typename B>
    auto& std::get(const Pair<A, B>& j) {
        static_assert(N == 0 || N == 1, "invalid template argument");
        if constexpr (N == 0) {
            return j.a;
        } else {
            return j.b;
        }
    }

    template <std::size_t N, typename A, typename B, typename C>
    auto& std::get(const Triplet<A, B, C>& j) {
        static_assert(N == 0 || N == 1 || N == 2, "invalid template argument");
        if constexpr (N == 0) {
            return j.a;
        } else if constexpr (N == 1) {
            return j.b;
        } else if constexpr (N == 2) {
            return j.c;
        }
    }
}

template <std::size_t N, typename I, typename A, typename B>
auto& std::get(const Pair<A, B>& j, I i) {
    static_assert(N == 0 || N == 1, "invalid template argument");
    if constexpr (N == 0) {
        return j.a(i);
    } else {
        return j.b(i);
    }
}

template <std::size_t N, typename I, typename A, typename B, typename C>
auto& std::get(const Triplet<A, B, C>& j, I i) {
    static_assert(N == 0 || N == 1 || N == 2, "invalid template argument");
    if constexpr (N == 0) {
        return j.a(i);
    } else if constexpr (N == 1) {
        return j.b(i);
    } else if constexpr (N == 2) {
        return j.c(i);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
// Assignment functions. These are used in the overload of the copy assignment operator

// from number literal
template <
    std::size_t n,
    typename S,
    typename T,
    typename std::enable_if<std::is_arithmetic_v<T>, int>::type = 0>
inline void _assign(S& s, const T& val) {
    // we might be storing a number as a field of the Pair object
    // so we just want this to be a single operation
    // check if the type returned by get<n>(s) is an arithmetic or not
    using Ts = std::remove_reference_t<decltype(get<n>(s))>;
    if constexpr (std::is_arithmetic_v<Ts>) {
        std::get<n>(s) = val;
    } else {
        // this cover the case where we are setting a Pair object
        // to a number, e.g. to set all fields to zero
        for (auto i = 0; i != get<n>(s).size(); i++)
            get<n>(s, i) = val;
    }
}

// from an expression
template <
    std::size_t n,
    typename S,
    typename E>
inline void _assign(S& s, const CoupledExpr<E>& _expr) {
    const E& expr(_expr);
    // check if the type returned by get<n>(s) is an arithmetic or not
    using Ts = std::remove_reference_t<decltype(get<n>(s))>;
    if constexpr (std::is_arithmetic_v<Ts>) {
        std::get<n>(s) = get<n>(expr);
    } else {
        for (auto i = 0; i != get<n>(s).size(); i++)
            get<n>(s, i) = get<n>(expr, i);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
// Definitions of the two objects

template <typename A, typename B>
struct Pair : public CoupledExpr<Pair<A, B>> {
    A a;
    B b;

    // constructor from the three elements
    Pair(A&& _a, B&& _b)
        : a(std::forward<A>(_a))
        , b(std::forward<B>(_b)) {
    }

    // define copy constrcutor so we can instantiate an array of Pair objects
    Pair(const Pair<A, B>& ab)
        : a(ab.a)
        , b(ab.b) {
    }

    template <typename E>
    inline Pair<A, B>& operator=(const E& val) {
        _assign<0>(*this, val);
        _assign<1>(*this, val);
        return *this;
    }
};

template <typename A, typename B, typename C>
struct Triplet : public CoupledExpr<Triplet<A, B, C>> {
    A a;
    B b;
    C c;

    // constructor from the three elements
    Triplet(A&& _a, B&& _b, C&& _c)
        : a(std::forward<A>(_a))
        , b(std::forward<B>(_b))
        , c(std::forward<C>(_c)) {
    }

    // define copy constrcutor so we can instantiate an array of Triplet objects
    Triplet(const Triplet<A, B, C>& abc)
        : a(abc.a)
        , b(abc.b)
        , c(abc.c) {
    }

    template <typename E>
    inline Triplet<A, B, C>& operator=(const E& val) {
        _assign<0>(*this, val);
        _assign<1>(*this, val);
        _assign<2>(*this, val);
        return *this;
    }
};

//////////////////////////////////////////////////////////////////////////////////////////
// These are the two main functions to be used.
//
// This takes two objects by reference and is used when we want
// these objects to be modified in place and packed in a lightweight
// wrapper object, for instance for forward integration
template <typename A, typename B>
Pair<A&, B&> refcouple(A& a, B& b) {
    return { a, b };
}

template <typename A, typename B, typename C>
Triplet<A&, B&, C&> refcouple(A& a, B& b, C& c) {
    return { a, b, c };
}

// This one takes two objects by value. If they are lvalues, we make
// copies at the call site and then move them to a new object. If they
// are rvalues, the compiler will call the move constructor to create
// the input arguments from the rvalues and then we wimply move these
// objects into the Pair or Triplet. This function is used to create
// a new Pair object with independent inner members, and is primarily
// used to create appropriate storage copies in, e.g. the RK4 method
template <typename A, typename B>
Pair<A, B> couple(A a, B b) {
    return { std::move(a), std::move(b) };
}

template <typename A, typename B, typename C>
Triplet<A, B, C> couple(A a, B b, C c) {
    return { std::move(a), std::move(b), std::move(c) };
}

//////////////////////////////////////////////////////////////////////////////////////////
// Now define the objects used for the expression templates using Pair structs. We
// define * and / of Pair and Triplet objects with arithmetic types only, i.e. we model
// a vector space. Note that we allow division by a Pair or Triplet, because this
// leads to a shorter code and it is not to be used in user code
#define _DEFINE_MULDIV_OPERATOR(_Op, _Name)                 \
                                                            \
    template <typename ARG, typename S>                     \
    struct _Name : public CoupledExpr<_Name<ARG, S>> {      \
        const ARG& arg;                                     \
        const S    s;                                       \
        _Name(const ARG& _arg, const S& _s)                 \
            : arg(_arg)                                     \
            , s(_s) {}                                      \
    };                                                      \
                                                            \
    template <std::size_t n, typename ARG, typename S>      \
    inline auto get(const _Name<ARG, S>& v) {               \
        return std::get<n>(v.arg) _Op v.s;                  \
    };                                                      \
                                                            \
    template <std::size_t n, typename ARG, typename S>      \
    inline auto get(const _Name<ARG, S>& v, int i) {        \
        return get<n>(v.arg, i) _Op v.s;                    \
    };                                                      \
                                                            \
    template <typename ARG, typename S>                     \
    inline _Name<ARG, S>                                    \
    operator _Op(const CoupledExpr<ARG>& arg, const S& s) { \
        static_assert(std::is_arithmetic_v<S>);             \
        return { arg, s };                                  \
    }                                                       \
                                                            \
    template <typename ARG, typename S>                     \
    inline _Name<ARG, S>                                    \
    operator _Op(const S& s, const CoupledExpr<ARG>& arg) { \
        static_assert(std::is_arithmetic_v<S>);             \
        return { arg, s };                                  \
    }

_DEFINE_MULDIV_OPERATOR(*, CoupledMul)
_DEFINE_MULDIV_OPERATOR(/, CoupledDiv)

#undef _DEFINE_MULDIV_OPERATOR

//////////////////////////////////////////////////////////////////////////////////////////
// Define Addition and Division for Pair object
#define _DEFINE_ADDSUB_OPERATOR(_Op, _Name)                                      \
                                                                                 \
    template <typename ARG1, typename ARG2>                                      \
    struct _Name : public CoupledExpr<_Name<ARG1, ARG2>> {                       \
        const ARG1& arg1;                                                        \
        const ARG2& arg2;                                                        \
                                                                                 \
        _Name(const ARG1& _arg1, const ARG2& _arg2)                              \
            : arg1(_arg1)                                                        \
            , arg2(_arg2) {}                                                     \
    };                                                                           \
                                                                                 \
    template <std::size_t n, typename ARG, typename S>                           \
    inline auto get(const _Name<ARG, S>& v, int i) {                             \
        return get<n>(v.arg1, i) _Op get<n>(v.arg2, i);                          \
    }                                                                            \
                                                                                 \
    template <std::size_t n, typename ARG, typename S>                           \
    inline auto get(const _Name<ARG, S>& v) {                                    \
        return std::get<n>(v.arg1) _Op std::get<n>(v.arg2);                      \
    }                                                                            \
                                                                                 \
    template <typename ARG1, typename ARG2>                                      \
    inline _Name<ARG1, ARG2>                                                     \
    operator _Op(const CoupledExpr<ARG1>& arg1, const CoupledExpr<ARG2>& arg2) { \
        return { arg1, arg2 };                                                   \
    }

_DEFINE_ADDSUB_OPERATOR(+, CoupledAdd)
_DEFINE_ADDSUB_OPERATOR(-, CoupledSub)

#undef _DEFINE_ADDSUB_OPERATOR
}