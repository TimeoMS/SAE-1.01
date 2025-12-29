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
    unsigned abs;
    unsigned ord;
}; // une position dans la grille

void couleur(const unsigned &coul)
{
    cout << "\033[" << coul << "m";
}

void fond(const unsigned &coul)
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
        for (size_t y(0); y < size; ++y)
            for (size_t x(0); x < size; ++x)
                at(x, y) = rand() % KNbCandies;
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
};

int main(int argc, char const *argv[])
{
    srand(time(0));
    Matrix m(3);
    m.displayGrid();

#ifndef NDEBUG
    cout << AllocationCount << '\n';
#endif

    return 0;
}