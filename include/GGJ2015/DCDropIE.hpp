#ifndef GGJ2015_DCDROPIE
#define GGJ2015_DCDROPIE

#include "../GGJ2015/DCCommon.hpp"
#include "../GGJ2015/DCDrop.hpp"
#include "../GGJ2015/DCInstantEffect.hpp"

namespace ggj
{
	struct DropIE : public Drop
	{
		std::vector<InstantEffect> ies;
		ssvs::BitmapTextRich btr{*getAssets().fontObStroked};

		inline DropIE(GameSession& mGameSession) : Drop{mGameSession} { }

		inline void ieToRichText(InstantEffect& mX)
		{
			auto& cl(btr.mk<BP::ClFG>(sfc::Green));
			cl.setAnimPulse(0.05f, 110);

			if(mX.type == InstantEffect::Type::Sub) cl.setColor(sfc::Red);

			btr << cl << mX.getStrType()
				<< ssvu::toStr(static_cast<int>(mX.value)) << " "
				<< sfc::White << mX.getStrStat() << "\n";
		}

		inline void recalculateText()
		{
			btr.clear();
			btr.setAlign(ssvs::TextAlign::Center);
			btr << btr.mk<BP::Trk>(-3);

			for(auto& x : ies) ieToRichText(x);
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

		inline void update(FT mFT) override
		{
			btr.update(mFT);
		}

		inline void draw(ssvs::GameWindow& mGW, const Vec2f& mPos, const Vec2f& mCenter) override
		{
			Drop::draw(mGW, mPos, mCenter);
			ssvs::setOrigin(btr, ssvs::getLocalCenter);
			btr.setPosition(card.getPosition());
			mGW.draw(btr);

		}
	};
}

#endif
