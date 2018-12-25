#pragma once
#include <array>
#include <stdexcept>

namespace Flows {

template <size_t N>
class Tableau{
private:
    std::array<std::array<double, N>, N> a;
               std::array<double, N>     b;
               std::array<double, N>     c;
public:
    Tableau(std::array<std::array<double, N>, N> a,
                       std::array<double, N>     b,
                       std::array<double, N>     c)
        : a (a) , b (b) , c (c) {}

    inline double operator () (char abc, int j, int k) const {
        return a[j][k];
    }

    inline double operator () (char bc, int k) const {
        switch (bc) {
            case 'b': return b[k];
            case 'c': return c[k];
        }
        throw std::invalid_argument("You must be joking!");
    }
};

template <size_t N>
class IMEXTableau{
private:
    Tableau<N> IM;
    Tableau<N> EX;
public:
    IMEXTableau(Tableau<N> IM, Tableau<N> EX)
        : IM (IM) , EX (EX) {}

    inline double operator () (char imex, char a, int j, int k) const {
        switch (imex) {
            case 'I': return IM('a', j, k);
            case 'E': return EX('a', j, k);
        }
        throw std::invalid_argument("You must be joking!");
    }
    inline double operator () (char imex, char bc, int k) const {
        switch (imex) {
            case 'I': return IM(bc, k);
            case 'E': return EX(bc, k);
        }
        throw std::invalid_argument("You must be joking!");
    }
};


////////////////////////////////////////////////////////////////
// Define tableaux as global variables

// second order method
static const Tableau<3> _CB2I = {{0.0 / 1.0, 0.0 / 1.0, 0.0 / 1.0,
                                  0.0 / 1.0, 2.0 / 5.0, 0.0 / 1.0,
                                  0.0 / 1.0, 5.0 / 6.0, 1.0 / 6.0},
                                 {0.0 / 1.0, 5.0 / 6.0, 1.0 / 6.0},
                                 {0.0 / 1.0, 2.0 / 5.0, 1.0 / 1.0}};
static const Tableau<3> _CB2E = {{0.0 / 1.0, 0.0 / 1.0, 0.0 / 1.0,
                                  2.0 / 5.0, 0.0 / 1.0, 0.0 / 1.0,
                                  0.0 / 1.0, 1.0 / 1.0, 0.0 / 1.0},
                                 {0.0 / 1.0, 5.0 / 6.0, 1.0 / 6.0},
                                 {0.0 / 1.0, 2.0 / 5.0, 1.0 / 1.0}};
static const IMEXTableau<3> CB2{_CB2I, _CB2E};

// third order method
static const Tableau<4> _CB3eI = {{0.0 / 1.0, 0.0 / 1.0, 0.0 / 1.0, 0.0 / 1.0,
                                   0.0 / 1.0, 1.0 / 3.0, 0.0 / 1.0, 0.0 / 1.0,
                                   0.0 / 1.0, 1.0 / 2.0, 1.0 / 2.0, 0.0 / 1.0,
                                   0.0 / 1.0, 3.0 / 4.0, -1.0 / 4.0, 1.0 / 2.0},
                                  {0.0 / 1.0, 3.0 / 4.0, -1.0 / 4.0, 1.0 / 2.0},
                                  {0.0 / 1.0, 1.0 / 3.0, 1.0 / 1.0, 1.0 / 1.0}};
static const Tableau<4> _CB3eE = {{0.0 / 1.0, 0.0 / 1.0, 0.0 / 1.0, 0.0 / 1.0,
                                   1.0 / 3.0, 0.0 / 1.0, 0.0 / 1.0, 0.0 / 1.0,
                                   0.0 / 1.0, 1.0 / 1.0, 0.0 / 1.0, 0.0 / 1.0,
                                   0.0 / 1.0, 3.0 / 4.0, 1.0 / 4.0, 0.0 / 1.0},
                                  {0.0 / 1.0, 3.0 / 4.0, -1.0 / 4.0, 1.0 / 2.0},
                                  {0.0 / 1.0, 1.0 / 3.0, 1.0 / 1.0, 1.0 / 1.0}};
static const IMEXTableau<4> CB3e{_CB3eI, _CB3eE};

}