#pragma once

#include <valarray>

/* alias */
using vec3 = std::valarray<double>;

/* unpack vectors */
std::tuple<double, double, double> unpack(const vec3& u) {
    return { u[0], u[1], u[2] };
}

/* linear term functions */
inline void _mul(vec3& dudt, const vec3& u) {
    dudt[0] = -10.0 * u[0];
    dudt[1] = -1.0 * u[1];
    dudt[2] = -8.0 / 3.0 * u[2];
}

inline void _mul(vec3& dudt, const vec3& u, double c) {
    dudt[0] = u[0] / (1.0 - c * (-10.0));
    dudt[1] = u[1] / (1.0 - c * (-1.0));
    dudt[2] = u[2] / (1.0 - c * (-8.0 / 3.0));
}

/* Nonlinear equations */
// We split the equations into explicit and implicit components, for testing purposes.
struct Lorenz {
    int _split;

    Lorenz(int split = 0)
        : _split(split) {}

    void operator()(double t, const vec3& u, vec3& dudt) {
        auto [x, y, z] = unpack(u);

        dudt[0] = 10.0 * (y - x) - _split * (-10.0 * x);
        dudt[1] = 28.0 * x - y - x * z - _split * (-y);
        dudt[2] = -8.0 / 3.0 * z + x * y - _split * (-8.0 / 3.0 * z);
    }

    inline void mul(vec3& dudt, const vec3& u) {
        _mul(dudt, u);
    }

    inline void mul(vec3& dudt, const vec3& u, double c) {
        _mul(dudt, u, c);
    }
};

struct LorenzTan {
    int _split;

    LorenzTan(int split = 0)
        : _split(split) {}

    void operator()(double t, const vec3& u, const vec3& dudt, const vec3& v, vec3& dvdt) {
        auto [x, y, z]    = unpack(u);
        auto [xp, yp, zp] = unpack(v);

        dvdt[0] = 10.0 * (yp - xp) - _split * (-10.0 * xp);
        dvdt[1] = (28.0 - z) * xp - yp - x * zp - _split * (-yp);
        dvdt[2] = -8.0 / 3.0 * zp + x * yp + xp * y - _split * (-8.0 / 3.0 * zp);
    }
    inline void mul(vec3& dudt, const vec3& u) {
        _mul(dudt, u);
    }

    inline void mul(vec3& dudt, const vec3& u, double c) {
        _mul(dudt, u, c);
    }
};

struct LorenzAdj {
    int _split;

    LorenzAdj(int split = 0)
        : _split(split) {}

    void operator()(double t, const vec3& u, const vec3& v, vec3& dvdt) {
        auto [x, y, z]    = unpack(u);
        auto [xp, yp, zp] = unpack(v);

        dvdt[0] = -10.0 * xp - (z - 28.0) * yp + y * zp - _split * (-10.0 * xp);
        dvdt[1] = 10.0 * xp - yp + x * zp - _split * (-yp);
        dvdt[2] = -x * yp - 8.0 / 3.0 * zp - _split * (-8.0 / 3.0 * zp);
    }

    inline void mul(vec3& dudt, const vec3& u) {
        _mul(dudt, u);
    }

    inline void mul(vec3& dudt, const vec3& u, double c) {
        _mul(dudt, u, c);
    }
};
