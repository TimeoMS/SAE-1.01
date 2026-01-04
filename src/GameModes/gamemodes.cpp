#include "gamemodes.h"
#include "Matrix/matrix.h"
#include "Matrix/constants.h"
#include <sstream>

bool readInput(size_t &x, size_t &y, char &direction)
{
    std::string line;
    if (!std::getline(std::cin, line))
        return false;

    std::istringstream iss(line);

    // Correction ici :
    if (iss >> x >> y >> direction)
    {
        return true;
    }
    return false;
}

void GameModes::normal()
{
    Matrix m(5);
    unsigned score(0);
    for (;;)
    {
        size_t x(0);
        size_t y(0);
        char direction('d');

        m.displayGrid();

        std::cout << "x y direction (u d l r, q pour quitter) (votre score est de " << score << "): ";

        if (!readInput(x, y, direction))
        {
            casali::couleur(KRouge);
            std::cerr << "Erreur de saisie\n";
            casali::couleur(KReset);
            continue;
        }

        if (direction == 'q')
            break;

        if (direction != 'u' && direction != 'd' && direction != 'r' && direction != 'l')
            std::cerr << "Direction n est pas bien utilise\n";

        m.makeAMove({x, y}, direction);

        casali::maPosition pos{0, 0};

        for (size_t x(0); x < m.n; ++x)
        {
            unsigned howMany(0);
            if (m.atLeastThreeInAColumnFrom(x, pos, howMany))
            {
                score += howMany;
                m.removalInColumn(pos, howMany);
            }
        }

        for (size_t x(0); x < m.n; ++x)
        {
            unsigned howMany(0);
            if (m.atLeastThreeInARowFrom(x, pos, howMany))
            {
                score += howMany;
                m.removalInRow(pos, howMany);
            }
        }
    }
}