#ifndef CASALI_H
#define CASALI_H

#include <cstddef>

namespace casali
{
    struct maPosition
    {
        size_t abs;
        size_t ord;
    }; // une position dans la grille

    void couleur(unsigned coul);

    void fond(unsigned coul);

    void clearScreen();
}

#endif