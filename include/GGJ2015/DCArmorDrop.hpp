#ifndef GGJ2015_DCARMORDROP
#define GGJ2015_DCARMORDROP

#include "../GGJ2015/DCCommon.hpp"

namespace ggj
{
    struct ArmorDrop final : public Drop
    {
        Armor armor;
        ArmorStatsDraw asd;
        sf::Sprite armorSprite;

        inline ArmorDrop(GameSession& mGameSession) : Drop{mGameSession}
        {
            card.setTexture(*getAssets().equipCard);
        }

        inline void apply(Creature& mX) override
        {
            getAssets().soundPlayer.play(*getAssets().equipArmor);
            mX.armor = armor;
        }

        inline void draw(ssvs::GameWindow& mGW, const Vec2f& mPos,
            const Vec2f& mCenter) override
        {
            Drop::draw(mGW, mPos, mCenter);

            armorSprite.setTexture(*getAssets().armDrop);
            ssvs::setOrigin(armorSprite, ssvs::getLocalCenter);
            armorSprite.setPosition(card.getPosition());
            mGW.draw(armorSprite);

            asd.pos = Vec2f{30 - 16, 30 + 6};
            asd.draw(armor, mGW, mPos, mCenter);
        }
    };
}

#endif
