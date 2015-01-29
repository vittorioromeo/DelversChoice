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

		float x(static_cast<float>(*statPtr));

		switch(type)
		{
			case Type::Add: *statPtr += value; break;
			case Type::Sub: *statPtr -= value; break;
			case Type::Mul: *statPtr = static_cast<int>(x * value); break;
			case Type::Div: *statPtr = static_cast<int>(x / value); break;
		}

		eventLo() << "Got " << getStrType() << ssvu::toStr(static_cast<int>(value)) << " " << getStrStat() << "!\n";

		mX.checkBurns(mGameSession);
	}
}

#endif
