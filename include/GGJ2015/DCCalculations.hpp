#ifndef GGJ2015_DCCALCULATIONS
#define GGJ2015_DCCALCULATIONS

#include "../GGJ2015/DCCommon.hpp"

namespace ggj
{
struct Calculations
{
    inline static bool isWeaponStrongAgainst(const Weapon& mW, const Armor& mA)
    {
        return (mW.strongAgainst & mA.elementTypes).any();
    }

    inline static bool isWeaponWeakAgainst(const Weapon& mW, const Armor& mA)
    {
        return (mW.weakAgainst & mA.elementTypes).any();
    }

    inline static auto getWeaponDamageAgainst(
    const Weapon& mW, const Armor& mA, ATK mBonusATK, DEF mBonusDEF)
    {
        auto result((mW.atk + mBonusATK) - (mA.def + mBonusDEF));
        if(isWeaponStrongAgainst(mW, mA)) result *= Constants::bonusMultiplier;
        if(isWeaponWeakAgainst(mW, mA)) result *= Constants::malusMultiplier;
        return ssvu::getClampedMin(result, 0);
    }

    inline static bool canWeaponDamage(
    const Weapon& mW, const Armor& mA, ATK mBonusATK, DEF mBonusDEF)
    {
        return getWeaponDamageAgainst(mW, mA, mBonusATK, mBonusDEF) > 0;
    }
};
}

#endif
