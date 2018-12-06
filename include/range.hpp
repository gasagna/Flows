#pragma once
#include <tuple>
#include <algorithm>

namespace Flows {

class TimeIterator {
private:
    double t, t_from, t_to, dt;
    long int i = 0;

public:
    TimeIterator(double t_from, double t_to, double dt)
        : t_from (t_from), t_to (t_to), dt (dt) {}

    std::tuple<double, double> operator *() const {
        double dt_loc = t_to > t_from ? 
                        std::min(t_to, t + dt) - t: 
                        std::max(t_to, t + dt) - t;
        double t = t_from + i * dt_loc;
        return {t, dt_loc};
    }

    const TimeIterator& operator ++ () {
        i++;
        return *this;
    }
    
    bool operator != (const TimeIterator& other) const {
        return other.t_to != t_from + i*dt;
    }
};

class TimeRange {
private:
    TimeIterator t_curr_it, t_to_it;
    
public:
    TimeRange(double t_from, double t_to, double dt) 
        : t_curr_it (TimeIterator(t_from, t_to, dt))
        , t_to_it   (TimeIterator(t_to,   t_to, dt)) {

        if (dt < 0 and t_to > t_from) {
            throw std::invalid_argument("t_to cannot be larger "
                "than t_from for backward integration");
        }
        if (dt > 0 and t_to < t_from) {
            throw std::invalid_argument("t_from cannot be larger "
                "than t_to for forward integration");
        }
    }

    TimeIterator begin() { return t_curr_it; }
    TimeIterator end()   { return t_to_it;   }
};
}