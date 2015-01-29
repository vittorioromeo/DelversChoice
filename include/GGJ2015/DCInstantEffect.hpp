#ifndef GGJ2015_DCINSTANTEFFECT
#define GGJ2015_DCINSTANTEFFECT

#include "../GGJ2015/DCCommon.hpp"

namespace ggj
{
	struct InstantEffect
	{
		enum class Type : int
		{
			Add = 0,
			Sub = 1,
			Mul = 2,
			Div = 3
		};

		enum class Stat : int
		{
			SHPS = 0,
			SATK = 1,
			SDEF = 2
		};

		Type type;
		Stat stat;
		float value;

		inline InstantEffect(Type mType, Stat mStat, float mValue) : type{mType}, stat{mStat}, value{mValue} { }
		inline void apply(GameSession& mGameSession, Creature& mX);

		inline const auto& getStrType()
		{
			static auto array(ssvu::makeArray
			(
				"+",
				"-",
				"*",
				"/"
			));

			return array[static_cast<int>(type)];
		}

		inline const auto& getStrStat()
		{
			static auto array(ssvu::makeArray
			(
				"HPS",
				"ATK",
				"DEF"
			));

			return array[static_cast<int>(stat)];
		}
	};
}

#endif
