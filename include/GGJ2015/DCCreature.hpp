#ifndef GGJ2015_DCCREATURE
#define GGJ2015_DCCREATURE

#include "../GGJ2015/DCCommon.hpp"

namespace ggj
{
    struct Creature
    {
        std::string name{"Unnamed"};
        Weapon weapon;
        Armor armor;
        HPS hps{-1};

        ATK bonusATK{0};
        DEF bonusDEF{0};

        inline void attackOnce(Creature& mX)
        {
            auto dmg(Calculations::getWeaponDamageAgainst(
                weapon, mX.armor, bonusATK, mX.bonusDEF));
            mX.hps -= dmg;
        }

        void checkBurns(GameSession& mGameSession);

        inline void fight(Creature& mX)
        {
            eventLo() << name << " engages " << mX.name << "!\n";
            auto hpsBefore(hps);
            auto xHPSBefore(mX.hps);

            while(true)
            {
                attackOnce(mX);
                if(mX.isDead()) break;

                mX.attackOnce(*this);
                if(isDead()) break;
            }

            if(isDead())
                eventLo() << mX.name << " wins. HPS " << xHPSBefore << " -> "
                          << mX.hps << "!\n";
            else
                eventLo() << name << " wins. HPS " << hpsBefore << " -> " << hps
                          << "!\n";
        }

        inline bool canDamage(Creature& mX) const noexcept
        {
            return Calculations::canWeaponDamage(
                weapon, mX.armor, bonusATK, mX.bonusDEF);
        }

        inline bool isDead() const noexcept { return hps <= 0; }

        inline std::string getLogStr() const
        {
            std::string result;

            result += "HPS: " + ssvu::toStr(hps) + ", ";
            result += "ATK: " + ssvu::toStr(weapon.atk) + ", ";
            result += "DEF: " + ssvu::toStr(armor.def) + ", ";
            result += "Str: " + ssvu::toStr(weapon.strongAgainst) + ", ";
            result += "Wkk: " + ssvu::toStr(weapon.weakAgainst);

            return result;
        }
    };
}

#endif
