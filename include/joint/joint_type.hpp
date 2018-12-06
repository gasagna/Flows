#pragma once
#include <utility>
#include <tuple>

#include "joint_expr.hpp"
#include "joint_oper.hpp"

namespace Flows {

/////////////////////////////////////////////////////////////////////////////////////////
// This is just a wrapper around a tuple, to pack a number of arguments into an 
// heterogeneous container. This support arithmetic operations using expression 
// templates defined in "joint_oper.hpp"
template <typename ... ARGS>
struct Joint : public JointExpr<Joint<ARGS...>> {
    std::tuple<ARGS...> args;
    Joint( ARGS& ... _args) : args(_args...) {}
    Joint(ARGS&& ... _args) : args(std::forward<ARGS>(_args)...) {}

    /////////////////////////////////////////////////////////////////////////////////////
    // Assign from JointExpr
    template <
        std::size_t n,
        template <typename...> typename E,
        typename ... EARGS>
    inline void _assign_nth(const JointExpr<E<EARGS...>>& _expr) {
        const E<EARGS...>& expr(_expr);
            // std::get<n>(args) = std::get<n>(expr)();
        auto& lhs      = std::get<n>(args); 
        const auto rhs = std::get<n>(expr);
        int N = lhs.size();
        for (int i = 0; i != N; i++)
            lhs[i] = rhs[i];
        if constexpr (n+1 < sizeof...(ARGS))
            _assign_nth<n+1>(_expr);
    }

    // // Assign from number
    // template <
    //     std::size_t n,
    //     typename T,
    //     typename std::enable_if<std::is_arithmetic_v<T>, int>::type = 0>
    // inline void _assign_nth(const T& val) {
    //     std::get<n>(args) = val;
    //     if constexpr (n+1 < sizeof...(ARGS))
    //         _assign_nth<n+1>(val);
    // }

    ////////////////////////////////////////////////////////////////
    // Assignment. This will call _assign_nth recursively on every
    // element of the args tuple, using the appropriate specialisation
    // based on the type of tuple element.
    template <typename E>
    inline Joint<ARGS...>& operator = (const E& _expr) {
        _assign_nth<0>(_expr);
        return *this;
    }
};

}

////////////////////////////////////////////////////////////////
// Joints are gettable arguments, like tuples
namespace std {
    template <std::size_t n, typename ... ARGS>
    auto get(const Flows::Joint<ARGS...>& j) {
        return std::get<n>(j.args);
    }
}