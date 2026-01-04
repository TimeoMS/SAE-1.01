#include "matrix.h"
#include "constants.h"
#include <iostream>
#include <iomanip>
#include <random>

using std::cout, casali::maPosition;

/**
 * @brief genere une matrice 1D avec des chiffre aléatoire sans triplet
 *
 *  */
Matrix::Matrix(size_t size) : n(size), m(size * size)
{

    for (;;)
    {
        bool hasTriplet(false);
        for (size_t i(0); i < m.size(); ++i)
            m[i] = rand() % KNbCandies;

        maPosition pos{0, 0};
        unsigned count(0);

        for (size_t i(0); i < n; ++i)
            if (atLeastThreeInAColumnFrom(i, pos, count))
                hasTriplet = true;

        for (size_t i(0); i < n; ++i)
            if (atLeastThreeInARowFrom(i, pos, count))
                hasTriplet = true;
        if (!hasTriplet)
            break;
    }
};

/**
 * @brief recherche un triplé ou plus en colonne dans une matrice
 * @param x, pos, howmany
 *
 * x et la position ou chercher pos est ou sauvegarder la position exacte et howmany et le nombre exact de chiffre identique
 *  */
bool Matrix::atLeastThreeInAColumnFrom(size_t x, maPosition &pos, unsigned &howMany) const
{
    size_t startY(0);
    unsigned count(1);
    size_t index(x);
    unsigned previousVal = m[index];
    index += n;

    for (size_t y(1); y < n; ++y)
    {
        unsigned currentVal = m[index];
        if (previousVal == currentVal && currentVal != KImpossible)
            ++count;
        else
        {
            if (count >= 3)
            {
                pos = {x, startY};
                howMany = count;
                return true;
            }

            previousVal = currentVal;

            startY = y;
            count = 1;
        }
        index += n;
    }

    if (count >= 3)
    {
        pos = {x, startY};
        howMany = count;
        return true;
    }

    return false;
}

/**
 * @brief recherche un triplé ou plus en ligne dans une matrice
 * @param y, pos, howmany
 *
 * y et la position ou chercher pos est ou sauvegarder la position exacte et howmany et le nombre exact de chiffre identique
 *  */
bool Matrix::atLeastThreeInARowFrom(size_t y, maPosition &pos, unsigned &howMany) const
{

    size_t startX(0);
    unsigned count(1);
    size_t index(y * n);
    unsigned previousVal = m[index];
    index += 1;

    for (size_t x(1); x < n; ++x)
    {
        unsigned currentVal = m[index];
        if (previousVal == currentVal && currentVal != KImpossible)
            ++count;
        else
        {
            if (count >= 3)
            {
                pos = {startX, y};
                howMany = count;
                return true;
            }

            previousVal = currentVal;

            startX = x;
            count = 1;
        }
        ++index;
    }

    if (count >= 3)
    {
        pos = {startX, y};
        howMany = count;
        return true;
    }

    return false;
}

/**
 * @brief permet de changer les posisiont de deux element d'une matrice en fonction de l'imput joueur
 * @param pos, direction
 *
 * pos est la position de l'element a échangé et direction est vers ou l'envoyé
 *  */
void Matrix::makeAMove(const maPosition &pos, char direction)
{

    size_t targetOrd = pos.ord;
    size_t targetAbs = pos.abs;

    switch (direction)
    {
    case 'r':
        if (targetAbs + 1 < n)
            targetAbs++;
        break;
    case 'l':
        if (targetAbs > 0)
            targetAbs--;
        break;
    case 'u':
        if (targetOrd > 0)
            targetOrd--;
        break;
    case 'd':
        if (targetOrd + 1 < n)
            targetOrd++;
        break;
    default:
        return;
    }

    std::swap(at(pos), at(targetAbs, targetOrd));
}

/**
 * @brief affiche une matrice coloré en foction des chiffre la composant
 *
 *  */
void Matrix::displayGrid() const
{
    casali::couleur(KReset);
    cout << "--------------\n";
    for (size_t y(0); y < n; ++y)
    {
        std::cout << '|';
        for (size_t x(0); x < n; ++x)
            casali::couleur(colorMap[m[y * n + x]]),
                cout << std::setw(2) << m[y * n + x] << ' ';
        casali::couleur(KReset);
        cout << "|\n";
    }
}

/**
 * @brief echange la position de deux element
 * @param pos1, pos2
 *
 * les pos sont les position a échanger
 *  */
void Matrix::switch2posColumn(const casali::maPosition &pos1, const casali::maPosition &pos2)
{
    size_t index(pos2.ord * n + pos1.abs);
    size_t indexFinal(pos1.ord * n + pos1.abs);

    for (; index > indexFinal;)
    {
        std::swap(m[index], m[index - n]);
        index -= n;
    }
}

/**
 * @brief supprime les élément dans une colonne
 * @param pos, howMany
 *
 * les pos sont les position a échanger
 *  */
void Matrix::removalInColumn(const maPosition &pos, unsigned howMany)
{

    for (size_t i(0); i < howMany; ++i)
    {
        unsigned currentOrd = pos.ord + i;

        maPosition holePos = {pos.abs, currentOrd};
        maPosition topOfThisColumn = {pos.abs, 0};

        m[currentOrd * n + pos.abs] = KImpossible;
        switch2posColumn(topOfThisColumn, holePos);
    }
}

/**
 * @brief supprime les élément dans une ligne
 * @param pos, howMany
 *
 * les pos sont les position a échanger
 *  */
void Matrix::removalInRow(const maPosition &pos, unsigned howMany)
{

    for (size_t i = 0; i < howMany; ++i)
    {
        unsigned currentAbs = pos.abs + i;

        maPosition holePos = {currentAbs, pos.ord};
        maPosition topOfThisColumn = {currentAbs, 0};

        m[pos.ord * n + currentAbs] = KImpossible;

        switch2posColumn(topOfThisColumn, holePos);
    }
}