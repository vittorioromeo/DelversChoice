#ifndef GGJ2015_DCWEAPONDROP
#define GGJ2015_DCWEAPONDROP

#include "../GGJ2015/DCCommon.hpp"

namespace ggj
{
struct WeaponDrop : public Drop
{
    Weapon weapon;
    WeaponStatsDraw wsd;
    sf::Sprite typeSprite;

    inline WeaponDrop(GameSession& mGameSession) : Drop{mGameSession}
    {
        card.setTexture(*getAssets().equipCard);
    }

    inline void apply(Creature& mX) override
    {
        getAssets().soundPlayer.play(*getAssets().equipWpn);
        mX.weapon = weapon;
    }

    inline void draw(
    ssvs::GameWindow& mGW, const Vec2f& mPos, const Vec2f& mCenter) override
    {
        Drop::draw(mGW, mPos, mCenter);

        typeSprite.setTexture(weapon.getTypeTexture());
        ssvs::setOrigin(typeSprite, ssvs::getLocalCenter);
        typeSprite.setPosition(card.getPosition());
        mGW.draw(typeSprite);

        wsd.pos = Vec2f{30 - 16, 30 + 6};
        wsd.draw(weapon, mGW, mPos, mCenter);
    }
};
}

#endif
