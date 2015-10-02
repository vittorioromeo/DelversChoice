#ifndef GGJ2015_DCCREATURE_INL
#define GGJ2015_DCCREATURE_INL

#include "../GGJ2015/DCCommon.hpp"

namespace ggj
{
inline void Creature::checkBurns(GameSession& mGameSession)
{
    int burn{0};

    if(bonusATK < 0) {
        burn -= bonusATK;
        bonusATK = 0;
    }

    if(bonusDEF < 0) {
        burn -= bonusDEF;
        bonusDEF = 0;
    }

    if(burn == 0) return;

    // TODO: calc
    auto x(burn * (mGameSession.gd.difficulty) / 10.f);

    hps -= x;
    eventLo() << name << " suffers " << x << " stat burn dmg!\n";
}
}

#endif
