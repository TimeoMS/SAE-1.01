#include "casali.h"

#include <iostream>

void casali::couleur(unsigned coul)
{
    std::cout << "\033[" << coul << "m";
}

void casali::fond(unsigned coul)
{
    std::cout << "\033[" << coul + 10 << "m";
}

void casali::clearScreen()
{
    std::cout << "\033[H\033[2J";
}