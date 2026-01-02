#ifndef MATRIX_H
#define MATRIX_H

#include <vector>
#include "Casali/casali.h"
#include "../constants.h"

struct Matrix // Pour l'optimisation
{

    size_t n;
    std::vector<unsigned> m;

    Matrix(size_t size);

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

    bool atLeastThreeInARowFrom(size_t y, casali::maPosition &pos, unsigned &howMany) const;
};

#endif