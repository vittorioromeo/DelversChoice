#ifndef GGJ2015_DCCHOICEITEMDROP_INL
#define GGJ2015_DCCHOICEITEMDROP_INL

#include "../GGJ2015/DCCommon.hpp"

namespace ggj
{
	inline ChoiceItemDrop::ChoiceItemDrop(GameSession& mGS, SizeT mIdx) : Choice{mGS, mIdx}
	{
		drops.setTexture(*getAssets().drops);
		itemDrops = mGS.generateDrops();
	}
	inline void ChoiceItemDrop::execute()
	{
		getAssets().soundPlayer.play(*getAssets().grab);
		gameSession.startDrops(&itemDrops);
		gameSession.resetChoiceAt(idx, ssvu::mkUPtr<ChoiceAdvance>(gameSession, idx));
	}
	inline void ChoiceItemDrop::draw(ssvs::GameWindow& mGW, const Vec2f& mPos, const Vec2f&)
	{
		drops.setPosition(mPos);
		mGW.draw(drops);
	}
}

#endif
