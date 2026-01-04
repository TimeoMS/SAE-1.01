#include "GameModes/gamemodes.h"
#include <random>
#include <ctime>

int main(int argc, char const *argv[])
{
    srand(time(0));
    GameModes::normal();
    return 0;
}