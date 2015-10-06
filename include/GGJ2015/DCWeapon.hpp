#ifndef GGJ2015_DCWEAPON
#define GGJ2015_DCWEAPON

#include "../GGJ2015/DCCommon.hpp"

namespace ggj
{
    struct Weapon
    {
        enum class Type : int
        {
            Mace = 0,
            Sword = 1,
            Spear = 2
        };

        std::string name{"Unarmed"};
        ElementBitset strongAgainst;
        ElementBitset weakAgainst;
        ATK atk{-1};
        Type type{Type::Mace};

        inline auto& getTypeTexture()
        {
            static auto array(ssvu::mkArray(getAssets().wpnMace,
                getAssets().wpnSword, getAssets().wpnSpear));

            return *array[ssvu::castEnum(type)];
        }

        inline auto& getTypeSoundBufferVec()
        {
            static auto array(ssvu::mkArray(getAssets().maceSnds,
                getAssets().swordSnds, getAssets().spearSnds));

            return array[ssvu::castEnum(type)];
        }

        inline void playAttackSounds()
        {
            auto& vec(getTypeSoundBufferVec());

            // Normal
            if(strongAgainst.none())
            {
                getAssets().soundPlayer.play(*vec[0]);
            }
            else
            {
                for(auto i(0u); i < Constants::elementCount; ++i)
                {
                    if(strongAgainst[i])
                        getAssets().soundPlayer.play(*vec[i + 1]);
                }
            }
        }
    };
}

#endif
