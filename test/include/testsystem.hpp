#pragma once

struct ImplicitTerm {
    double _lambda;

    ImplicitTerm(double lambda)
        : _lambda(lambda) {}

    // implicit term A = 0: solves (I - cA)*z = y
    inline void mul(double& z, const double y, double c) {
        z = y / (1 - c * _lambda);
    }

    // implicit term A = 0: calculates out = A*x
    inline void mul(double& out, const double x) {
        out = x * _lambda;
    }
};

struct ExplicitTerm {
    double _lambda;

    ExplicitTerm(double lambda)
        : _lambda(lambda) {}

    // explicit term
    inline void operator()(double t, double x, double& dxdt) {
        dxdt = _lambda * x;
    }
};
