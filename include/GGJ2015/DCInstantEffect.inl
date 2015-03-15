#ifndef GGJ2015_DCINSTANTEFFECT_INL
#define GGJ2015_DCINSTANTEFFECT_INL

#include "../GGJ2015/DCCommon.hpp"

namespace ggj
{
	inline void InstantEffect::apply(GameSession& mGameSession, Creature& mX)
	{
		StatType* statPtr{nullptr};

		switch(stat)
		{
			case Stat::SHPS: statPtr = &mX.hps; break;
			case Stat::SATK: statPtr = &mX.bonusATK; break;
			case Stat::SDEF: statPtr = &mX.bonusDEF; break;
		}

		float x(ssvu::toFloat(*statPtr));

		switch(type)
		{
			case Type::Add: *statPtr += value; break;
			case Type::Sub: *statPtr -= value; break;
			case Type::Mul: *statPtr = ssvu::toInt(x * value); break;
			case Type::Div: *statPtr = ssvu::toInt(x / value); break;
		}

		eventLo() << "Got " << getStrType() << ssvu::toStr(ssvu::toInt(value)) << " " << getStrStat() << "!\n";

		mX.checkBurns(mGameSession);
	}
}

#endif
