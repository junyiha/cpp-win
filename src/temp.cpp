#include "temp.h"

void test_global_variable()
{
    for (int i = 0; i < 6; i++)
    {
        std::cerr << POSITION_HOME_ARR[i] << "\n";
    }
}