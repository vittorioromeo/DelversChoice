#ifndef GGJ2015_ITEMDROPS
#define GGJ2015_ITEMDROPS

#include "../GGJ2015/DCCommon.hpp"

namespace ggj
{
	struct ItemDrops
	{
		ssvu::UPtr<Drop> drops[Constants::maxDrops];

		inline ItemDrops()
		{
			for(auto i(0u); i < Constants::maxDrops; ++i)
				drops[i] = nullptr;
		}

		inline bool has(int mIdx)
		{
			return drops[mIdx] != nullptr;
		}

		inline void give(int mIdx, Creature& mX)
		{
			drops[mIdx]->apply(mX);
			drops[mIdx].release();
		}

		inline void update(FT mFT)
		{
			for(auto& p : drops) if(p != nullptr) p->update(mFT);
		}
	};
}

#endif
