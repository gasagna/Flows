#pragma once
#include <utility>
#include <tuple>

#include "joint_expr.hpp"
#include "joint_type.hpp"

////////////////////////////////////////////////////////////////
// Forward declarations from joint_type.hpp
namespace Flows {
    template <typename ... ARGS>
    struct Joint;
}

namespace std {
    template <std::size_t n, typename ... ARGS>
    auto get(const Flows::Joint<ARGS...>& j);
}

//////////////////////////////////////////////////////////////////////////////////////
// Generate code for division ad multiplication of a Joint by a scalar using a macro
// For simplicity 
#define _DEFINE_MULDIV_OPERATOR(_Op, _Name)                                          \
    namespace Flows {                                                                \
        template <typename S, typename ARG>                                          \
        struct _##_Name {                                                            \
            const S      s;                                                          \
            const ARG  arg;                                                          \
            _##_Name(S _s, const ARG&  _arg) : s(_s), arg(                  _arg)  {}\
            inline auto operator [] (int i) const { return arg[i] _Op s; }           \
            inline auto operator () (     ) const { return arg    _Op s; }           \
        };                                                                           \
                                                                                     \
        template <typename ... ARGS>                                                 \
        struct _Name : public JointExpr<_Name<ARGS...>> {                            \
            const std::tuple<ARGS...> args;                                          \
            _Name(const ARGS& ... _args) : args(    _args...) {}                     \
        };                                                                           \
                                                                                     \
        template <                                                                   \
            typename S,                                                              \
            template <typename...> typename E,                                       \
            typename ... ARGS>                                                       \
        auto operator _Op (const JointExpr<E<ARGS...>>& _arg, S s) {                 \
            return _helper_##_Name(s, _arg, std::index_sequence_for<ARGS...>{});     \
        }                                                                            \
                                                                                     \
        template <                                                                   \
            typename S,                                                              \
            template <typename...> typename E,                                       \
            typename ... ARGS>                                                       \
        auto operator _Op (S s, const JointExpr<E<ARGS...>>& _arg) {                 \
            return _helper_##_Name(s, _arg, std::index_sequence_for<ARGS...>{});     \
        }                                                                            \
                                                                                     \
        template <                                                                   \
            typename S,                                                              \
            template <typename...> typename E,                                       \
            typename ... ARGS,                                                       \
            std::size_t... I>                                                        \
        auto _helper_##_Name(S s,                                                    \
                             const JointExpr<E<ARGS...>>& _arg,                      \
                             std::index_sequence<I...>) {                            \
            static_assert(std::is_arithmetic_v<S>);                                  \
            const E<ARGS...>& arg(_arg);                                             \
            return _Name<_##_Name<S, ARGS>...>(_##_Name(s, std::get<I>(arg))...);    \
        }                                                                            \
    }                                                                                \
                                                                                     \
    namespace std {                                                                  \
        template <std::size_t n, typename ... ARGS>                                  \
        auto get(const Flows::_Name<ARGS...>& j) {                                   \
            return std::get<n>(j.args);                                              \
        }                                                                            \
    }                                                                                \

_DEFINE_MULDIV_OPERATOR(*, JointMul)
_DEFINE_MULDIV_OPERATOR(/, JointDiv)

#undef _DEFINE_MULDIV_OPERATOR

//////////////////////////////////////////////////////////////////////////////////////
// Generate code for addition/subtraction
#define _DEFINE_ADDSUB_OPERATOR(_Op, _Name)                                           \
    namespace Flows {                                                                 \
        template <typename ARG1, typename ARG2>                                       \
        struct _##_Name {                                                             \
            const ARG1  arg1;                                                         \
            const ARG2  arg2;                                                         \
            _##_Name(const ARG1& _arg1, const ARG2& _arg2)                            \
                : arg1(_arg1), arg2(_arg2) {}                                         \
            _##_Name(ARG1&& _arg1, ARG2&& _arg2)                                      \
                : arg1(std::forward<ARG1>(_arg1)), arg2(std::forward<ARG2>(_arg2)) {} \
            inline auto operator [] (int i) const { return arg1[i] _Op arg2[i]; }     \
            inline auto operator () (     ) const { return arg1()  _Op arg2();  }     \
        };                                                                            \
                                                                                      \
        template <typename ... ARGS>                                                  \
        struct _Name : public JointExpr<_Name<ARGS...>> {                             \
            std::tuple<ARGS...> args;                                                 \
            _Name( ARGS& ... _args) : args(std::forward<ARGS>(_args)...) {}           \
            _Name(ARGS&& ... _args) : args(std::forward<ARGS>(_args)...) {}           \
        };                                                                            \
                                                                                      \
        template <                                                                    \
            typename ... ARGS1, template <typename...> typename E1,                   \
            typename ... ARGS2, template <typename...> typename E2>                   \
        auto operator _Op (const JointExpr<E1<ARGS1...>>& _arg1,                      \
                           const JointExpr<E2<ARGS2...>>& _arg2) {                    \
            static_assert(sizeof...(ARGS1) == sizeof...(ARGS2));                      \
            return _helper_##_Name(_arg1, _arg2, std::index_sequence_for<ARGS1...>{});\
        }                                                                             \
                                                                                      \
        template <                                                                    \
            typename ... ARGS1, template <typename...> typename E1,                   \
            typename ... ARGS2, template <typename...> typename E2,                   \
            std::size_t... I>                                                         \
        auto _helper_##_Name(const JointExpr<E1<ARGS1...>>& _arg1,                    \
                             const JointExpr<E2<ARGS2...>>& _arg2,                    \
                             std::index_sequence<I...>) {                             \
            const E1<ARGS1...>& arg1(_arg1);                                          \
            const E2<ARGS2...>& arg2(_arg2);                                          \
            return _Name<_##_Name<ARGS1, ARGS2>...>(_##_Name(std::get<I>(arg1),       \
                                                             std::get<I>(arg2))...);  \
        }                                                                             \
    }                                                                                 \
                                                                                      \
    namespace std {                                                                   \
        template <std::size_t n, typename ... ARGS>                                   \
        auto get(const Flows::_Name<ARGS...>& j) {                                    \
            return std::get<n>(j.args);                                               \
        }                                                                             \
    }

_DEFINE_ADDSUB_OPERATOR(+, JointAdd)
_DEFINE_ADDSUB_OPERATOR(-, JointSub)

#undef _DEFINE_ADDSUB_OPERATOR