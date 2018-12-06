#pragma once

namespace Flows {

////////////////////////////////////////////////////////////////
// Abstract class for all time stepping techniques. Derived classes
// define different methods for the time step selection, e.g. using
// a constant time step, of with variable time stepping. These classes
// do not implement a standard interface, but are mostly used for dispatch,
// to select a specific implementation
class AbstractTimeStepping {};

////////////////////////////////////////////////////////////////
// Use constant time step throughout
class TimeStepConstant : public AbstractTimeStepping {
public:
    double dt;
    TimeStepConstant(double _dt) : dt (_dt) {}
};

}
