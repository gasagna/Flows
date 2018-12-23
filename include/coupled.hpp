#pragma once
#include <type_traits>
#include <utility>

namespace Flows {

//////////////////////////////////////////////////////////////////////////////////////////
// All objects will inherit from this, in order ot avoid polluting the namespace
// and avoiding catch all situations that you end up with using expression templates
template <typename E>
struct CoupledExpr {
    operator const E&() const {
        return *static_cast<const E*>(this);
    }
};

//////////////////////////////////////////////////////////////////////////////////////////
// forward declaration of the main type to pack the fields
template <typename A, typename B, typename C>
struct Coupled;

// define useful aliases for dispatch
template <typename A, typename B>
using Pair = Coupled<A, B, std::nullptr_t>;
template <typename A, typename B, typename C>
using Triplet = Coupled<A, B, C>;

// This function returns the _ArgN th field of the Coupled struct. We define up to
// three fields, as this shouls cover most use cases of this library
template <std::size_t n, typename A, typename B, typename C>
struct _get_helper {};

template <std::size_t n, typename A, typename B, typename C>
struct _get_helper_i {};

template <std::size_t n, typename... ARGS>
auto& get(const Coupled<ARGS...>& j) {
    return _get_helper<n, ARGS...>::arg(j);
}

template <std::size_t n, typename I, typename... ARGS>
auto& get(const Coupled<ARGS...>& j, I i) {
    static_assert(std::is_integral_v<I>);
    return _get_helper_i<n, ARGS...>::arg(j, i);
}

//////////////////////////////////////////////////////////////////////////////////////////
// Assignment functions. These are used in the overload of the copy assignment operator
//
// from number literal
template <
    std::size_t n,
    typename S,
    typename T,
    typename std::enable_if<std::is_arithmetic_v<T>, int>::type = 0>
inline void _assign(S& s, const T& val) {
    // we might be storing a number as a field of the Coupled object
    // so we just want this to be a single operation
    // check if the type returned by get<n>(s) is an arithmetic or not
    using Ts = std::remove_reference_t<decltype(get<n>(s))>;
    if constexpr (std::is_arithmetic_v<Ts>) {
        get<n>(s) = val;
    } else {
        // this cover the case where we are setting a Coupled object
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
        get<n>(s) = get<n>(expr);
    } else {
        for (auto i = 0; i != get<n>(s).size(); i++)
            get<n>(s, i) = get<n>(expr, i);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
template <typename A, typename B, typename C>
struct Coupled : public CoupledExpr<Coupled<A, B, C>> {
    A a;
    B b;
    C c;
    Coupled(A&& _a, B&& _b, C&& _c)
        : a(std::forward<A>(_a))
        , b(std::forward<B>(_b))
        , c(std::forward<C>(_c)) {}
    template <typename E>
    inline Coupled<A, B, C>& operator=(const E& val) {
        _assign<0>(*this, val);
        _assign<1>(*this, val);
        // we might have stored a null pointer in the third field, so we avoid
        if constexpr (not std::is_null_pointer<C>())
            _assign<2>(*this, val);
        return *this;
    }
};

// these are the main functions to be used
template <typename A, typename B>
Coupled<A, B, std::nullptr_t> couple(A&& a, B&& b) {
    return { std::forward<A>(a), std::forward<B>(b), nullptr };
}

template <typename A, typename B, typename C>
Coupled<A, B, C> couple(A&& a, B&& b, C&& c) {
    return { std::forward<A>(a), std::forward<B>(b), std::forward<C>(c) };
}

//////////////////////////////////////////////////////////////////////////////////////////
// Define class specialisations of _get_helper and _get_helper_i, to extract the
// fields of the Coupled struct.
#define _DEFINE_GET_OPERATOR(_ArgN, _F)                                                  \
                                                                                         \
    template <typename A, typename B, typename C>                                        \
    struct _get_helper<_ArgN, A, B, C> {                                                 \
        static constexpr auto& arg(const Coupled<A, B, C>& j) { return j._F; }           \
    };                                                                                   \
                                                                                         \
    template <typename A, typename B, typename C>                                        \
    struct _get_helper_i<_ArgN, A, B, C> {                                               \
        static constexpr auto& arg(const Coupled<A, B, C>& j, int i) { return j._F[i]; } \
    };

_DEFINE_GET_OPERATOR(0, a)
_DEFINE_GET_OPERATOR(1, b)
_DEFINE_GET_OPERATOR(2, c)

#undef _DEFINE_GET_OPERATOR

//////////////////////////////////////////////////////////////////////////////////////////
// Now define the objects used for the expression templates using Coupled structs. We
// define * and / of Coupled objects with arithmetic types only, i.e. we model Coupled
// as a vector space. Note that we allow S/Coupled, because this leads to a shorter code
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
        return get<n>(v.arg) _Op v.s;                       \
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
// Define Addition and Division for Coupled object
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
        return get<n>(v.arg1) _Op get<n>(v.arg2);                                \
    }                                                                            \
                                                                                 \
    template <typename ARG1, typename ARG2>                                      \
    inline _Name<ARG1, ARG2>                                                     \
    operator _Op(const CoupledExpr<ARG1>& arg1, const CoupledExpr<ARG2>& arg2) { \
        return { arg1, arg2 };                                                   \
    }

_DEFINE_ADDSUB_OPERATOR(+, CoupleAdd)
_DEFINE_ADDSUB_OPERATOR(-, CoupleSub)

#undef _DEFINE_ADDSUB_OPERATOR
}