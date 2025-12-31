#include <iostream>
#include <iomanip>
#include <ctime>
#include <random>
#include <vector>

using std::cout, std::vector;

#ifndef NDEBUG
unsigned AllocationCount(0);

void *operator new(size_t size)
{
    ++AllocationCount;
    return malloc(size);
}
#endif

constexpr unsigned KReset(0);
constexpr unsigned KNoir(30);
constexpr unsigned KRouge(31);
constexpr unsigned KVert(32);
constexpr unsigned KJaune(33);
constexpr unsigned KBleu(34);
constexpr unsigned KMAgenta(35);
constexpr unsigned KCyan(36);
constexpr unsigned KMaxTimes(3);
constexpr unsigned KNbCandies(10);

struct maPosition
{
    size_t abs;
    size_t ord;
}; // une position dans la grille

void couleur(unsigned coul)
{
    cout << "\033[" << coul << "m";
}

void fond(unsigned coul)
{
    cout << "\033[" << coul + 10 << "m";
}

void clearScreen()
{
    cout << "\033[H\033[2J";
}

struct Matrix // Pour l'optimisation
{

    size_t n;
    vector<unsigned> m;

    Matrix(size_t size) : n(size), m(size * size)
    {
        for (size_t i(0); i < m.size(); ++i)
            m[i] = rand() % KNbCandies;
    };

    void displayGrid() const
    {
        clearScreen();
        couleur(KReset);

        for (size_t y(0); y < n; ++y)
        {
            cout << '|';
            for (size_t x(0); x < n; ++x)
            {
                cout << std::setw(2) << m[y * n + x] << ' ';
            }
            cout << "|\n";
        }
    }

    unsigned &at(const maPosition &pos)
    {
        return m[pos.ord * n + pos.abs];
    }

    const unsigned &at(const maPosition &pos) const
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

    void makeAMove(const maPosition &pos, char direction)
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

    bool atLeastThreeInAColumnFrom(size_t x, maPosition &pos, unsigned &howMany)
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
};

int main(int argc, const char *argv[])
{
    srand(time(0));
    Matrix m(4);

#ifndef NDEBUG
    cout << "----------------\nNombre allocation: " << AllocationCount << '\n';
#endif

    return 0;
}