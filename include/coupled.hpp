#pragma once
#include <type_traits>
#include <utility>

namespace Flows {

//////////////////////////////////////////////////////////////////////////////////////////
// All objects will inherit from this, in order ot avoid polluting the namespace
// and avoiding catch all situations that you end up with using expression templates
template <typename E>
struct CoupledExpr {
    operator const E& () const {
        return *static_cast<const E*>(this);
    }
};

//////////////////////////////////////////////////////////////////////////////////////////
// forward declaration of the main type to pack the fields
template <typename A, typename B, typename C> struct Coupled;

// define useful aliases for dispatch
template <typename A, typename B>             using Pair    = Coupled<A, B, std::nullptr_t>;
template <typename A, typename B, typename C> using Triplet = Coupled<A, B, C>;

// these are the main functions to be used
template <typename A, typename B>
auto couple(A& a, B& b) {
    std::nullptr_t c = nullptr;
    return Coupled<A, B, std::nullptr_t>(a, b, c);
}

template <typename A, typename B, typename C>
auto couple(A& a, B& b, C& c) {
    return Coupled<A, B, C>(a, b, c);
}

// Use this helper struct to dispatch the call to the right getarg overload
template <std::size_t n> struct _arg {};

//////////////////////////////////////////////////////////////////////////////////////////
// Assignment functions. These are used in the overload of the copy assignment operator
//
// from number literal
template <
    std::size_t n,
    typename S,
    typename T,
    typename std::enable_if<std::is_arithmetic_v<T>, int>::type = 0
>
inline void _assign(S& s, const T& val, _arg<n> g) {
    // we might be storing a number as a field of the Coupled object
    // so we just want this to be a single operation
    if constexpr (std::is_arithmetic_v<S>) {
        s = val;
    } else {
        // this cover the case where we are setting a Coupled object
        // to a number, e.g. to set all fields to zero
        for (auto i = 0; i != s.size(); i++)
            s[i] = val;
    }
}

// from an expression
template <std::size_t n, typename S, typename E>
inline void _assign(S& s, const CoupledExpr<E>& _expr, _arg<n> g) {
    const E& expr(_expr);
    if constexpr (std::is_arithmetic_v<S>) {
        s = getarg(expr, g);
    } else {
        for (int i = 0; i != s.size(); i++)
            s[i] = getarg(expr, i, g);
    }
}

////////////////////////////////////////////////////////////////////////////////////////
// This function returns the _ArgN th field of the Coupled struct. We define up to
// three fields, as this shouls cover most use cases of this library
#define _DEFINE_GETARG_OPERATOR(_ArgN, _Name)                                            \
                                                                                         \
    template <typename ... ARGS>                                                         \
    inline auto getarg(const Coupled<ARGS...>& j, _arg<_ArgN>) {                             \
        return j._Name;                                                                  \
    }                                                                                    \
                                                                                         \
    template <typename ... ARGS>                                                         \
    inline auto getarg(const Coupled<ARGS...>& j, int i, _arg<_ArgN>) {                      \
        return j._Name[i];                                                               \
    }
 
_DEFINE_GETARG_OPERATOR(0, a)
_DEFINE_GETARG_OPERATOR(1, b)
_DEFINE_GETARG_OPERATOR(2, a)

#undef _DEFINE_GETARG_OPERATOR

//////////////////////////////////////////////////////////////////////////////////////////
template <typename A, typename B, typename C>
struct Coupled : public CoupledExpr <Coupled<A, B, C>>{
    A a; B b; C c;
    Coupled(const A& _a, const B& _b, const C& _c) : a(_a), b(_b), c(_c) {}
    template <typename E>
    inline Coupled<A, B, C>& operator = (const E& val) {
        _assign(a, val, _arg<0>());
        _assign(b, val, _arg<1>());
        // we might have stored a null pointer in the third field, so we avoid
        if constexpr (not std::is_null_pointer<C>())
            _assign(c, val, _arg<2>());
        return *this;
    }
};

//////////////////////////////////////////////////////////////////////////////////////////
#define _DEFINE_MULDIV_OPERATOR(_Op, _Name)                                              \
                                                                                         \
    template <typename ARG, typename S>                                                  \
    struct _Name : public CoupledExpr<_Name<ARG, S>> {                                   \
        const ARG& arg;                                                                  \
        const S      s;                                                                  \
        _Name(const ARG& _arg, const S& _s) : arg(_arg), s(_s) {}                        \
    };                                                                                   \
                                                                                         \
    template <std::size_t n, typename ARG, typename S>                                   \
    inline auto getarg(const _Name<ARG, S>& v, _arg<n> g) {                           \
        return getarg(v.arg, g) _Op v.s; };                                              \
                                                                                         \
    template <std::size_t n, typename ARG, typename S>                                   \
    inline auto getarg(const _Name<ARG, S>& v, int i, _arg<n> g) {                    \
        return getarg(v.arg, i, g) _Op v.s; };                                           \
                                                                                         \
    template <typename ARG, typename S>                                                   \
    inline _Name<ARG, S>                                                                 \
    operator _Op (const CoupledExpr<ARG>& arg, const S& s) {                             \
        static_assert(std::is_arithmetic_v<S>);                                          \
        return {arg, s};                                                                 \
    }                                                                                    \
                                                                                         \
    template <typename ARG, typename S>                                                   \
    inline _Name<ARG, S>                                                                 \
    operator _Op (const S& s, const CoupledExpr<ARG>& arg) {                             \
        static_assert(std::is_arithmetic_v<S>);                                          \
        return {arg, s};                                                                 \
    }                                                                                    \

_DEFINE_MULDIV_OPERATOR(*, CoupledMul)
_DEFINE_MULDIV_OPERATOR(/, CoupledDiv)

#undef _DEFINE_MULDIV_OPERATOR

//////////////////////////////////////////////////////////////////////////////////////////
#define _DEFINE_ADDSUB_OPERATOR(_Op, _Name)                                              \
                                                                                         \
    template <typename ARG1, typename ARG2>                                              \
    struct _Name : public CoupledExpr<_Name<ARG1, ARG2>> {                               \
        const ARG1& arg1;                                                                \
        const ARG2& arg2;                                                                \
                                                                                         \
        _Name(const ARG1& _arg1, const ARG2& _arg2) : arg1(_arg1), arg2(_arg2) {}        \
    };                                                                                   \
                                                                                         \
    template <std::size_t n, typename ARG, typename S>                                   \
    inline auto getarg(const _Name<ARG, S>& v, int i, _arg<n> g) {                    \
        return getarg(v.arg1, i, g) _Op getarg(v.arg2, i, g);                            \
    }                                                                                    \
                                                                                         \
    template <std::size_t n, typename ARG, typename S>                                   \
    inline auto getarg(const _Name<ARG, S>& v, _arg<n> g) {                           \
        return getarg(v.arg1, g) _Op getarg(v.arg2, g);                                  \
    }                                                                                    \
                                                                                         \
    template <typename ARG1, typename ARG2>                                               \
    inline _Name<ARG1, ARG2>                                                             \
    operator _Op (const CoupledExpr<ARG1>& arg1, const CoupledExpr<ARG2>& arg2) {        \
        return {arg1, arg2};                                                             \
    }                                                                                    \

_DEFINE_ADDSUB_OPERATOR(+, CoupleAdd)
_DEFINE_ADDSUB_OPERATOR(-, CoupleSub)

#undef _DEFINE_ADDSUB_OPERATOR

}