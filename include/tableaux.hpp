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

    inline double operator () (char imex, char a, int j, int k) {
        switch (imex) {
            case 'I': return IM('a', j, k);
            case 'E': return EX('a', j, k);
        }
        throw std::invalid_argument("You must be joking!");
    }
    inline double operator () (char imex, char bc, int k) {
        switch (imex) {
            case 'I': return IM(bc, k);
            case 'E': return EX(bc, k);
        }
        throw std::invalid_argument("You must be joking!");
    }
};

}