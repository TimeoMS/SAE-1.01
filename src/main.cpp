#include <iostream>
#include <iomanip>
#include <ctime>
#include <random>

using std::cout, std::vector, std::cin;

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
                at(x, y) = rand() % (KNbCandies - 1) + 1;
    };

    void displayGrid() const
    {
        clearScreen();
        couleur(KReset);

        for (size_t y = 0; y < n; ++y)
        {
            cout << '|';
            for (size_t x = 0; x < n; ++x)
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

        switch (tolower(direction))
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

    unsigned checkMatches()
    {
        // vecteur a optimiser pour stocker les positions
        vector<vector<bool>> toDelete(n, vector<bool>(n, false));

        // verification des lignes
        for (size_t y(0); y < n; ++y) {
            for (size_t x(0); x < n - 3; ++x) {
                if (at(x, y) != 0 && at(x, y) == at(x + 1, y) && at(x, y) == at(x + 2, y)) {
                    int lenth = x + 2;
                    while (lenth + 1 < n && at(x, y) == at(lenth + 1, y))
                        lenth++;
                    for (size_t i = x; i <= lenth; ++i)
                        toDelete[y][i] = true;
                    x = lenth;
                }
            }
        }

        // verification des colonnes
        for (size_t x(0); x < n; ++x) {
            for (size_t y(0); y < n - 3; ++y) {
                if (at(x, y) != 0 && at(x, y) == at(x, y + 1) && at(x, y) == at(x, y + 2)) {
                    int lenth = x + 2;
                    while (lenth + 1 < n && at(x, y) == at(x, lenth + 1))
                        lenth++;
                    for (size_t i = x; i <= lenth; ++i)
                        toDelete[i][x] = true;
                    y = lenth;
                }
            }
        }

        unsigned score = 0;
        // suppression des triples ou plus
        for (size_t y(0); y < n; ++y) {
            for (size_t x(0); x < n; ++x) {
                if (toDelete[y][x]) {
                    at(x, y) = 0;
                    score++;
                }
            }
        }
        return score;        
    }
};

void candyCrush(unsigned short nb_shots)
{
    srand(time(0));
    Matrix m(10);
    unsigned score = m.checkMatches();
    m.displayGrid();
    for (int i = 0; i < nb_shots; ++i) {
        maPosition pos;
        char direction;
        cout << "Enter Your Move (x y direction): ";
        cin >> pos.abs >> pos.ord >> direction;
        m.makeAMove(pos, direction);
        score += m.checkMatches();
        m.displayGrid();
        cout << "Shots left: " << nb_shots - i - 1 << "\n";
    }
    cout << "Game Over your score is: " << score << "\n";
}

int main(int argc, char const **argv)
{
    if (argc == 2 && std::string(argv[1]) == "-h") {
        cout << "Usage: \n"
             << "  ./candycrush [nb_shots](def: 10)    Start the game\n"
             << "  ./candycrush -h                     Show this help message\n";
        return 0;
    } else {
        candyCrush(argv[1] ? std::stoi(argv[1]) : 10);
    }
    return 0;
}