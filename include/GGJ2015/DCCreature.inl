#ifndef GGJ2015_DCCREATURE_INL
#define GGJ2015_DCCREATURE_INL

#include "../GGJ2015/DCCommon.hpp"

namespace ggj
{
	inline void Creature::checkBurns(GameSession& mGameSession)
	{
		int burn{0};

		if(bonusATK < 0)
		{
			burn -= bonusATK;
			bonusATK = 0;
		}

		if(bonusDEF < 0)
		{
			burn -= bonusDEF;
			bonusDEF = 0;
		}

		if(burn == 0) return;

		auto x(burn * (4 * mGameSession.roomNumber * mGameSession.difficulty));

		hps -= x;
		eventLo() << name << " suffers " << x << " stat burn dmg!\n";
	}
}

#endif
