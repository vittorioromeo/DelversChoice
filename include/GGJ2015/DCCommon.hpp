#ifndef GGJ2015_DCCOMMON
#define GGJ2015_DCCOMMON

#include "../GGJ2015/Common.hpp"
#include "../GGJ2015/Boilerplate.hpp"

namespace ggj
{
	namespace BP = ssvs::BTRP;
	using sfc = sf::Color;

	struct GameSession;

	using StatType = int;
	using HPS = StatType;
	using ATK = StatType;
	using DEF = StatType;

	struct Constants
	{
		static constexpr SizeT elementCount{4};
		static constexpr SizeT maxChoices{4};
		static constexpr SizeT maxDrops{3};
		static constexpr float bonusMultiplier{2.5f};
		static constexpr float malusMultiplier{0.8f};
	};

	using ElementBitset = std::bitset<Constants::elementCount>;

	template<typename TArg, typename... TArgs> inline auto mkShuffledVector(TArg&& mArg, TArgs&&... mArgs)
	{
		std::vector<TArg> result;
		result.reserve(1 + sizeof...(TArgs));
		result.emplace_back(ssvu::fwd<TArg>(mArg));
		ssvu::forArgs([&result](auto&& mX){ result.emplace_back(ssvu::fwd<decltype(mX)>(mX)); }, ssvu::fwd<TArgs>(mArgs)...);
		ssvu::shuffle(result);
		return result;
	}

	inline auto getStatDisplayStr(StatType mBase, StatType mBonus)
	{
		return ssvu::toStr(mBase + mBonus) + " (" + ssvu::toStr(mBase) + "+" + ssvu::toStr(mBonus) + ")";
	}
}

#endif
