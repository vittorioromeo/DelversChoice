#ifndef GGJ2015_DCCHOICECREATURE_INL
#define GGJ2015_DCCHOICECREATURE_INL

#include "../GGJ2015/DCCommon.hpp"

namespace ggj
{
	inline void ChoiceCreature::execute()
	{
		gameSession.player.weapon.playAttackSounds();

		if(gameSession.player.canDamage(creature))
		{
			gameSession.player.fight(creature);

			getAssets().soundPlayer.play(*getAssets().drop);
			gameSession.resetChoiceAt(idx, ssvu::makeUPtr<ChoiceItemDrop>(gameSession, idx));

			gameSession.shake = 10;
		}
		else
		{
			eventLo() << gameSession.player.name << " cannot fight " << creature.name << "!\n";
		}
	}
	inline void ChoiceCreature::draw(ssvs::GameWindow& mGW, const Vec2f& mPos, const Vec2f& mCenter)
	{
		Vec2f offset{4.f, 4.f};
		mGW.draw(enemySprite);
		enemySprite.setPosition(mCenter + Vec2f(0, std::sin(hoverRads) * 4.f));
		csd.draw(creature, mGW, offset + mPos, mCenter);
	}
}

#endif
