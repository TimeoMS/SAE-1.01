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

void Matrix::removalInColumn(maPosition &pos, unsigned howMany)
{
    for (size_t y = pos.ord; y < pos.ord + howMany; ++y)
    {
        at(pos.abs, y) = 0;
    }
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

void Matrix::removalInRow(maPosition &pos, unsigned howMany)
{
    for (size_t x = pos.abs; x < pos.abs + howMany; ++x)
    {
        at(x, pos.ord) = 0;
    }
}

void Matrix::reffill(int n)
{
    for (size_t x(0); x < n; ++x) {
        for (size_t y(n - 1); y < n; --y) {
            if (at(x, y) == 0) {
                size_t k = y;
                while (k > 0 && at(x, k) == 0)
                    k--;
                if (k == 0 && at(x, k) == 0) {
                    at(x, y) = rand() % (KNbCandies - 1) + 1;
                } else {
                    std::swap(at(x, y), at(x, k));
                }
            }
        }
    }
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
            cout << std::setw(2) << m[y * n + x] << ' ';
        cout << "|\n";
    }
}