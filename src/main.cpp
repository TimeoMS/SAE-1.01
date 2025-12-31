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
        Matrix m(4);
        casali::maPosition pos{0, 0};
        unsigned count(0);
        for (size_t i(0); i < 4; ++i)
            if (m.atLeastThreeInAColumnFrom(i, pos, count))
            {
                m.displayGrid();
                std::cout << "Il y a " << count << " nombres qui se suivent a (" << pos.abs << ',' << pos.ord << ")\n";
                return 0;
            }
    }

#ifndef NDEBUG
    std::cout << "----------------\nNombre allocation: " << AllocationCount << '\n';
#endif

    return 0;
}