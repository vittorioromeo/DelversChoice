#ifndef GGJ2015_DCDROPIE
#define GGJ2015_DCDROPIE

#include "../GGJ2015/DCCommon.hpp"

namespace ggj
{
	struct DropIE : public Drop
	{
		std::vector<InstantEffect> ies;
		std::vector<ssvs::BitmapText> bts;

		inline DropIE(GameSession& mGameSession) : Drop{mGameSession} { }

		inline void addIE(InstantEffect mIE)
		{
			ies.emplace_back(mIE);

			ssvs::BitmapText txt{mkTxtOBSmall()};
			txt.setString(mIE.getStrType() + ssvu::toStr(static_cast<int>(mIE.value)) + " " + mIE.getStrStat());
			ssvs::setOrigin(txt, ssvs::getLocalCenter);

			bts.emplace_back(txt);
		}

		inline void apply(Creature& mX) override
		{
			getAssets().soundPlayer.play(*getAssets().powerup, ssvs::SoundPlayer::Mode::Overlap, 1.8f);
			for(auto& x : ies) x.apply(gameSession, mX);
		}

		inline void draw(ssvs::GameWindow& mGW, const Vec2f& mPos, const Vec2f& mCenter) override
		{
			Drop::draw(mGW, mPos, mCenter);

			int i{0};
			for(auto& t : bts)
			{
				t.setPosition(card.getPosition() + Vec2f{0, -15.f + (10 * i)});
				mGW.draw(t);

				++i;
			}
		}
	};
}

#endif
