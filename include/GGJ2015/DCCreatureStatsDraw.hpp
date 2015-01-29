#ifndef GGJ2015_DCCREATURESTATSDRAW
#define GGJ2015_DCCREATURESTATSDRAW

#include "../GGJ2015/DCCommon.hpp"

namespace ggj
{
	struct CreatureStatsDraw
	{
		sf::Sprite iconHPS;
		ssvs::BitmapText txtHPS;

		WeaponStatsDraw wsd;
		ArmorStatsDraw asd;

		inline CreatureStatsDraw() : txtHPS{mkTxtOBSmall()}
		{
			iconHPS.setTexture(*getAssets().iconHPS);
		}

		inline void update(FT mFT) { wsd.update(mFT); asd.update(mFT); }

		inline void draw(Creature& mC, ssvs::GameWindow& mGW, const Vec2f& mPos, const Vec2f& mCenter)
		{
			txtHPS.setString(ssvu::toStr(mC.hps));
			iconHPS.setPosition(mPos + Vec2f{0.f, 12.f * 0.f});
			txtHPS.setPosition(iconHPS.getPosition() + Vec2f{12.f, 0});

			wsd.pos = Vec2f{0, 12.f};
			wsd.draw(mC, mGW, mPos, mCenter);

			asd.pos = Vec2f{0, wsd.eWK.getPosition().y - mPos.y + 12.f};
			asd.draw(mC, mGW, mPos, mCenter);

			mGW.draw(iconHPS);
			mGW.draw(txtHPS);
		}
	};
}

#endif
