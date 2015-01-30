#ifndef GGJ2015_DCDROPIE
#define GGJ2015_DCDROPIE

#include "../GGJ2015/DCCommon.hpp"

namespace ggj
{
	struct DropIE : public Drop
	{
		std::vector<InstantEffect> ies;
		ssvs::BitmapTextRich btr{*getAssets().fontObStroked};

		inline DropIE(GameSession& mGameSession) : Drop{mGameSession} { }

		inline void recalculateText()
		{
			// TODO: clear doesnt work
			// btr.clear();

			//btr = ssvs::BitmapTextRich{*getAssets().fontObStroked};
			btr.setAlign(ssvs::TextAlign::Center);
			btr << btr.mk<BP::Trk>(-3);

			for(auto& x : ies)
			{
				btr << sfc::Red << x.getStrType()
					<< sfc::White << ssvu::toStr(static_cast<int>(x.value)) << " "
					<< sfc::Cyan << x.getStrStat() << "\n";
			}


		}

		inline void addIE(InstantEffect mIE)
		{
			ies.emplace_back(mIE);
			recalculateText();
		}

		inline void apply(Creature& mX) override
		{
			getAssets().soundPlayer.play(*getAssets().powerup, ssvs::SoundPlayer::Mode::Overlap, 1.8f);
			for(auto& x : ies) x.apply(gameSession, mX);
		}

		inline void draw(ssvs::GameWindow& mGW, const Vec2f& mPos, const Vec2f& mCenter) override
		{
			Drop::draw(mGW, mPos, mCenter);
			//recalculateText();
			// TODO stuff
			ssvs::setOrigin(btr, ssvs::getLocalCenter);
			btr.setPosition(card.getPosition());
			mGW.draw(btr);

		}
	};
}

#endif
