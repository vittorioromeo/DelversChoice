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

        inline DropIE(GameSession& mGameSession) : Drop{mGameSession} {}

        inline void ieToRichText(InstantEffect& mX)
        {
            btr.getLast()
                .eff<BTR::PulseDef>(
                    mX.type == InstantEffect::Type::Sub ? sfc::Red : sfc::Green)
                .in(mX.getStrType())
                .in(ssvu::toStr(ssvu::toInt(mX.value)))
                .in(" ")
                .eff(sfc::White)
                .in(mX.getStrStat())
                .in("\n");
        }

        inline void recalculateText()
        {
            btr.clear();
            btr.setAlign(ssvs::TextAlign::Center);
            btr.eff<BTR::Tracking>(-3);

            for(auto& x : ies) ieToRichText(x);
        }

        inline void addIE(InstantEffect mIE)
        {
            ies.emplace_back(mIE);
            recalculateText();
        }

        inline void apply(Creature& mX) override
        {
            getAssets().soundPlayer.play(
                *getAssets().powerup, ssvs::SoundPlayer::Mode::Overlap, 1.8f);
            for(auto& x : ies) x.apply(gameSession, mX);
        }

        inline void update(FT mFT) override { btr.update(mFT); }

        inline void draw(ssvs::GameWindow& mGW, const Vec2f& mPos,
            const Vec2f& mCenter) override
        {
            Drop::draw(mGW, mPos, mCenter);
            ssvs::setOrigin(btr, ssvs::getLocalCenter);
            btr.setPosition(card.getPosition());
            mGW.draw(btr);
        }
    };
}

#endif
