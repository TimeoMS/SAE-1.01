#ifndef MATRIX_H
#define MATRIX_H

#include <iostream>
#include <vector>
#include "Casali/casali.h"
#include "../constants.h"

struct Matrix // Pour l'optimisation
{

    size_t n;
    std::vector<unsigned> m;

    Matrix(size_t size) : n(size), m(size * size)
    {
        for (size_t i(0); i < m.size(); ++i)
            m[i] = 1 + std::rand() % KNbCandies;
    };

    void displayGrid() const;

    unsigned &at(const casali::maPosition &pos)
    {
        return m[pos.ord * n + pos.abs];
    }

    const unsigned &at(const casali::maPosition &pos) const
    {
        return m[pos.ord * n + pos.abs];
    }

    unsigned &at(size_t x, size_t y)
    {
        return m[y * n + x];
    }

    const unsigned &at(size_t x, size_t y) const
    {
        return m[y * n + x];
    }

    void makeAMove(const casali::maPosition &pos, char direction);

    bool atLeastThreeInAColumnFrom(size_t x, casali::maPosition &pos, unsigned &howMany) const;

    void removalInColumn(casali::maPosition &pos, unsigned howMany);

    bool atLeastThreeInARowFrom(size_t y, casali::maPosition &pos, unsigned &howMany) const;

    void removalInRow(casali::maPosition &pos, unsigned howMany);

    void reffill(int n);
};

#endif