#include "../include/NetLayer/NetLayer.hpp"
#include "./utils/test_utils.hpp"

int main()
{
#undef NDEBUG
    ::nl::debugLo() << "abcd\n";

#define NDEBUG 1
    ::nl::debugLo() << "abcd\n";

    return 0;
}
