#include "GameModes/gamemodes.h"
#include <random>
#include <iostream>
#include <ctime>

int main(int argc, char const *argv[])
{
    srand(time(0));
    if (argc == 2 && std::string(argv[1]) == "-h") {
        std::cout << "Usage: \n"
             << "  ./candycrush [nb_shot](def: 4)                      Start the game\n"
             << "  ./candycrush -infinite                              Start the infinte game mode\n"
             << "  ./candycrush -versus [nb_shot](def: 4 per player)   Start the versus game mode\n"
             << "  ./candycrush -h                                     Show this help message\n";
        return 0;
    } else if (argc == 2 && std::string(argv[1]) == "-versus") {
        GameModes::versuslaunch(argv[2] ? std::stoi(argv[2]) : 10);
    } else if (argc == 2 && std::string(argv[1]) == "-infinite") {
        GameModes::infinite();
    } else {
        GameModes::normal(argv[1] ? std::stoi(argv[1]) : 20);
    }

    return 0;
}