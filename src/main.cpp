#include <iostream>
#include <iomanip>
#include <ctime>
#include <random>

using namespace std;

const unsigned KReset(0);
const unsigned KNoir(30);
const unsigned KRouge(31);
const unsigned KVert(32);
const unsigned KJaune(33);
const unsigned KBleu(34);
const unsigned KMAgenta(35);
const unsigned KCyan(36);
const unsigned KMaxTimes(3);
const unsigned KNbCandies(10);

typedef vector<unsigned> line; // un type représentant une ligne de la grille
typedef vector<line> mat;      // un type représentant la grille

struct maPosition
{
    unsigned abs;
    unsigned ord;
}; // une position dans la girlle

void initGrid(mat &grid, const size_t &matSize)
{
    grid.resize(matSize);
    srand(time(0));
    for (line &l : grid)
    {
        l.resize(matSize);
        for (unsigned &u : l)
            u = rand() % KNbCandies;
    }
}

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

/* UTILS */

void displayGrid(const mat &m)
{
    clearScreen();
    couleur(KReset);
    for (const line &l : m)
    {
        cout << '|';
        for (const unsigned &u : l)
            cout << setw(2) << u;
        cout << setw(2) << '|' << endl;
    }
}

/* UTILS */

int main(int argc, char const *argv[])
{
    mat m;
    initGrid(m, 4);
    displayGrid(m);
    return 0;
}