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

    for (;;)
    {
        Matrix m(6);
        casali::maPosition pos{0, 0};
        unsigned count(0);
        for (size_t x(0); x < 6; ++x)
            if (m.atLeastThreeInAColumnFrom(x, pos, count))
            {
                std::cout << "---------------------\n";
                m.displayGrid();
                std::cout << "---------------------\n";
                m.removalInColumn(pos, count);
                m.displayGrid();
            }
    }

#ifndef NDEBUG
    std::cout << "----------------\nNombre allocation: " << AllocationCount
              << '\n';
#endif

    return 0;
}
