#include "Matrix/matrix.h"
#include <iostream>
#include <sstream>
#include <fstream>

#ifndef NDEBUG
unsigned AllocationCount(0);

void *operator new(size_t size)
{
    ++AllocationCount;
    return malloc(size);
}
#endif

void checkscore(int score)
{
    std::ifstream file("highscore.txt");
    std::string buffer;
    
    if (!file.is_open()) {
        std::ofstream F("highscore.txt");
        F.close();
        file.open("highscore.txt", std::ios::in | std::ios::out);
    }
    if (file.peek() != std::ifstream::traits_type::eof()) {
        std::getline(file, buffer);
        file.close();
        std::ofstream file("highscore.txt", std::ios::trunc);
        int highscore (std::stoi(buffer));
        if (score > highscore) {
            file << score;
            std::cout << "Congratulation new highscore\n";
        } else {
            std::cout << "Your score is " << score << " the highscore is " << highscore << std::endl;
        }
    } else {
        file.close();
        std::ofstream file("highscore.txt", std::ios::trunc);
        file << score;
        std::cout << "Congratulation new highscore\n";
    }
    file.close();
}

void candyCrush(int n)
{
    srand(time(0));
    Matrix m(n);
    int score(0);

    bool exit(false);

    for (;;)
    {
        casali::maPosition pos{0, 0};
        unsigned count(0);
        std::string line;
        char direction;
        for (size_t i(0); i < n; ++i) {
            if (m.atLeastThreeInAColumnFrom(i, pos, count))
            {
                m.removalInColumn(pos, count);
                score += count;
            }
        }
        pos = {0, 0};
        count = 0;
        for (size_t i(0); i < n; ++i) {
            if (m.atLeastThreeInARowFrom(i, pos, count))
            {
                m.removalInRow(pos, count);
                score += count;
            }
        }
        m.reffill(n);
        m.displayGrid();
        std::cout << "Your current score is: " << score << '\n';
        std::cout << "Enter Your Move (x y direction): ";
        getline(std::cin, line);
        if (line == "exit") {
            break;
        }
        std::istringstream stream(line);
        stream >> pos.abs >> pos.ord >> direction;
        m.makeAMove(pos, direction);
    }
    checkscore(score);
}

int candyCrushVersus(Matrix &j, int &score, int n, int numplayer)
{
    casali::maPosition pos{0, 0};
    unsigned count(0);
    std::string line;
    char direction;
    for (size_t i(0); i < n; ++i) {
        if (j.atLeastThreeInAColumnFrom(i, pos, count))
        {
            j.removalInColumn(pos, count);
            score += count;
        }
    }
    pos = {0, 0};
    count = 0;
    for (size_t i(0); i < n; ++i) {
        if (j.atLeastThreeInARowFrom(i, pos, count))
        {
            j.removalInRow(pos, count);
            score += count;
        }
    }
    j.reffill(n);
    j.displayGrid();
    std::cout << "Your current score is: " << score << '\n';
    std::cout << "Enter Your Move Player " << numplayer << " (x y direction): ";
    getline(std::cin, line);
    if (line == "exit") {
        return 0;
    }
    std::istringstream stream(line);
    stream >> pos.abs >> pos.ord >> direction;
    j.makeAMove(pos, direction);
    return 1;
}

void candyCrushVersusLaunch(int n)
{
    srand(time(0));
    Matrix j1(n);
    Matrix j2 = j1;
    int scorej1(0);
    int scorej2(0);
    bool playerOneTurn(true);

    for (int i(0); i < 20; i += 1)
    {
        if (playerOneTurn){
            if (!candyCrushVersus(j1, scorej1, n, 1))
                goto endgame;
            playerOneTurn = false;
        } else {
            if (!candyCrushVersus(j2, scorej2, n, 2))
                goto endgame;
            playerOneTurn = true;
        }
    }
    endgame:
        std::cout << "Final Scores:\nPlayer 1: " << scorej1 << "\nPlayer 2: " << scorej2 << '\n';
}

int main(int argc, const char *argv[])
{
    if (argc == 2 && std::string(argv[1]) == "-h") {
        std::cout << "Usage: \n"
             << "  ./candycrush [map_size](def: 4)           Start the game\n"
             << "  ./candycrush -versus [map_size](def: 4)   Start the versus game mode\n"
             << "  ./candycrush -h                           Show this help message\n";
        return 0;
    } else if (argc == 2 && std::string(argv[1]) == "-versus") {
        candyCrushVersusLaunch(argv[2] ? std::stoi(argv[2]) : 4);
    } else {
        candyCrush(argv[1] ? std::stoi(argv[1]) : 4);
    }

// #ifndef NDEBUG
//     std::cout << "----------------\nNombre allocation: " << AllocationCount
//               << '\n';
// #endif

    return 0;
}
