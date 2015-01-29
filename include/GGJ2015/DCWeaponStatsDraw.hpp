#ifndef GGJ2015_DCWEAPONSTATSDRAW
#define GGJ2015_DCWEAPONSTATSDRAW

#include "../GGJ2015/DCCommon.hpp"

namespace ggj
{
	struct WeaponStatsDraw
	{
		Vec2f pos;
		sf::Sprite iconATK;
		sf::Sprite eST, eWK;
		StatRichText srtATK;

		inline WeaponStatsDraw()
		{
			iconATK.setTexture(*getAssets().iconATK);
			eST.setTexture(*getAssets().eST);
			eWK.setTexture(*getAssets().eWK);
		}

		inline void update(FT mFT) { srtATK.update(mFT); }

		inline void commonDraw(Weapon& mW, ssvs::GameWindow& mGW, const Vec2f& mPos, const Vec2f&)
		{
			iconATK.setPosition(mPos + pos);
			eST.setPosition(iconATK.getPosition() + Vec2f{0, 10 + 1});
			eWK.setPosition(eST.getPosition() + Vec2f{0, 6 + 1});
			srtATK.txt.setPosition(iconATK.getPosition() + Vec2f{12.f, 0});

			appendElems(mGW, eST, mW.strongAgainst);
			appendElems(mGW, eWK, mW.weakAgainst);

			mGW.draw(iconATK);
			mGW.draw(srtATK.txt);
			mGW.draw(eST);
			mGW.draw(eWK);
		}

		inline void draw(Weapon& mW, ssvs::GameWindow& mGW, const Vec2f& mPos, const Vec2f& mCenter)
		{
			srtATK.set(mW.atk);
			commonDraw(mW, mGW, mPos, mCenter);
		}

		inline void draw(Creature& mC, ssvs::GameWindow& mGW, const Vec2f& mPos, const Vec2f& mCenter)
		{
			srtATK.set(mC.weapon.atk, mC.bonusATK);
			commonDraw(mC.weapon, mGW, mPos, mCenter);
		}
	};
}

#endif
