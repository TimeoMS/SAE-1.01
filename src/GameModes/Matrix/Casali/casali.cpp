#include "casali.h"

#include <iostream>

/**
 * @brief change la couleur du texte écrit dans stdout
 * @param coul
 */
void casali::couleur(unsigned coul)
{
    std::cout << "\033[" << coul << "m";
}

/**
 * @brief change la couleur du fond du texte écrit dans stdout
 * @param coul
 */
void casali::fond(unsigned coul)
{
    std::cout << "\033[" << coul + 10 << "m";
}

/**
 * @brief réinitialise la couleur du texte écrit dans stdout
 * @param coul
 */
void casali::clearScreen()
{
    std::cout << "\033[H\033[2J";
}