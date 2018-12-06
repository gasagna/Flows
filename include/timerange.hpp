#pragma once
#include <tuple>
#include <cmath>

namespace Flows {

inline bool isForw(double t1, double t2) { return t2 > t1; }
inline bool isBack(double t1, double t2) { return t2 > t1; }

class TimeRangeIter { 
private:
    double t_from, t_to, dt;
    size_t i;
    
public:
    TimeRangeIter(double t_from, double t_to, double dt, size_t i)
        : t_from (t_from) , t_to (t_to), dt (dt), i (i) {}

    std::tuple<double, double> operator *() const {
        double  t_curr = isForw(t_from, t_to)
                       ? t_from + i * dt
                       : t_from - i * dt;
        double dt_curr = isForw(t_from, t_to)
                       ? std::min(t_curr + dt, t_to) - t_curr
                       : std::max(t_curr - dt, t_to) - t_curr;
        return {t_curr, dt_curr};
    }

    const TimeRangeIter& operator ++ () {
        i++; 
        return *this;
    }
    
    bool operator != (const TimeRangeIter& other) const {
        return other.i != i;
    }
};

class TimeRange {
public:
    double t_from, t_to, dt;
    size_t len;

public:
    TimeRange(double t_from, double t_to, double dt)
        : t_from (t_from)
        , t_to   (t_to)
        , dt     (dt)
        , len    (ceil(fabs(t_to - t_from)/dt)) {
        // add one further step if needed
        if (isForw(t_from, t_to) and t_from + len*dt < t_to)
            len += 1;
        if (isBack(t_from, t_to) and t_from - len*dt > t_to)
            len += 1;
    }
    
    TimeRangeIter begin() const { return {t_from, t_to, dt,   0}; }
    TimeRangeIter end()   const { return {t_from, t_to, dt, len}; }
};
}