#ifndef GGJ2015_DCARMOR
#define GGJ2015_DCARMOR

#include "../GGJ2015/DCCommon.hpp"

namespace ggj
{
struct Armor
{
    std::string name{"Unarmored"};
    ElementBitset elementTypes;
    DEF def{-1};
};
}

#endif
