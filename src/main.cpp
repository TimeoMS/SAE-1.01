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

int main(int argc, const char *argv[])
{
    srand(time(0));
    Matrix m(4);
    int score(0);

    for (;;)
    {
        casali::maPosition pos{0, 0};
        unsigned count(0);
        std::string line;
        char direction;
        for (size_t i(0); i < 4; ++i) {
            if (m.atLeastThreeInAColumnFrom(i, pos, count))
            {
                m.removalInColumn(pos, count);
                score += count;
            }
        }
        pos = {0, 0};
        count = 0;
        for (size_t i(0); i < 4; ++i) {
            if (m.atLeastThreeInARowFrom(i, pos, count))
            {
                m.removalInRow(pos, count);
                score += count;
            }
        }
        m.reffill(4);
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

// #ifndef NDEBUG
//     std::cout << "----------------\nNombre allocation: " << AllocationCount
//               << '\n';
// #endif

    return 0;
}
