#pragma once

namespace Flows {

/////////////////////////////////////////////////////////////////////////////////////////
// Base class of all expression with Joint objects. All subclass derived from this
template <typename E>
struct JointExpr {
    operator const E& () const {
        return *static_cast<const E*>(this);
    }
};


}