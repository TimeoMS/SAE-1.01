#include "Matrix/matrix.h"
#include <iostream>

#ifndef NDEBUG
unsigned AllocationCount(0);

void *operator new(size_t size)
{
    ++AllocationCount;
    return malloc(size);
}
#endif

int main(int argc, const char *argv[])
{
    srand(time(0));

    bool exit(false);

    for (; !exit;)
    {
        Matrix m(6);
        casali::maPosition pos{0, 0};
        unsigned count(0);
        for (size_t x(0); x < 6; ++x)
            if (m.atLeastThreeInARowFrom(x, pos, count) && count >= 5)
            {
                exit = true;
                std::cout << "---------------------\n";
                m.displayGrid();
                std::cout << "---------------------\n";
                m.removalInRow(pos, count);
                m.displayGrid();
            }
    }

#ifndef NDEBUG
    std::cout << "----------------\nNombre allocation: " << AllocationCount
              << '\n';
#endif

    return 0;
}
