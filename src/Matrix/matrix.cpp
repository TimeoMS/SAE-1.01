#include "matrix.h"
#include <iostream>
#include <iomanip>

using std::cout, casali::maPosition;

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
        if (previousVal == currentVal)
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
        if (previousVal == currentVal)
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

void Matrix::displayGrid() const
{
    casali::clearScreen();
    casali::couleur(KReset);

    for (size_t y(0); y < n; ++y)
    {
        std::cout << '|';
        for (size_t x(0); x < n; ++x)
        {
            cout << std::setw(2) << m[y * n + x] << ' ';
        }
        cout << "|\n";
    }
}