#include "catch.hpp"
#include <IMEXRKCB.hpp>
#include <valarray>
#include <iostream>
#include <tuple>
#include <cmath>

class LinearSystem {
private:
    double lambda;
public:
    explicit LinearSystem(double lambda)
             : lambda(lambda) {}

    // explicit term
    template <typename X>
    inline void exterm(double t, X& x, X& dxdt) {
        dxdt = lambda * x / 2;
    }

    // implicit term A = 0: solves (I - cA)*z = y
    template <typename X>
    inline void imterm(double c, X& y, X& z) {
        z = y / (1 - c*lambda/2); 
    }

    // implicit term A = 0: calculates out = A*x
    template <typename X>
    inline void imterm(X& out, X& x) {
        out = x * lambda/2;
    }
};

template <typename T, typename X>
void asymptotic_error(T& F, X& z0, int order, double value) {
    for (double dt : {0.1, 0.01, 0.001}) {
        z0[0] = 1.0;
        F(z0, 0, 1, dt);
        // error on final integration is one order less than method order
        REQUIRE(fabs(z0[0] - std::exp(1)) / pow(dt, order) < value);
    }
}

TEST_CASE("Testing integrator", "[integrator]") {
    
    // initial condition
    std::valarray<double> z0 = {1.0};

    // LinearSystem
    LinearSystem lf(1.0);

    // define integrators
    IMEXRKCB::Integrator<LinearSystem, std::valarray<double>, IMEXRKCB::CB2_3R2R>   F_CB2_3R2R(lf, z0, 1.0);
    IMEXRKCB::Integrator<LinearSystem, std::valarray<double>, IMEXRKCB::CB3e_3R2R> F_CB3E_3R2R(lf, z0, 1.0);

    // check error convergence is consistent with method order
    asymptotic_error(F_CB2_3R2R,  z0, 2, 0.06792);
    asymptotic_error(F_CB3E_3R2R, z0, 3, 0.01889);
}