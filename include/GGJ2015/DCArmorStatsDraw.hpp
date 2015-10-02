#ifndef GGJ2015_DCARMORSTATSDRAW
#define GGJ2015_DCARMORSTATSDRAW

#include "../GGJ2015/DCCommon.hpp"

namespace ggj
{
struct ArmorStatsDraw
{
    Vec2f pos;
    sf::Sprite iconDEF;
    sf::Sprite eTY;
    StatRichText srtDEF;

    inline ArmorStatsDraw()
    {
        iconDEF.setTexture(*getAssets().iconDEF);
        eTY.setTexture(*getAssets().eTY);
    }

    inline void update(FT mFT) { srtDEF.update(mFT); }

    inline void commonDraw(
    Armor& mA, ssvs::GameWindow& mGW, const Vec2f& mPos, const Vec2f&)
    {
        iconDEF.setPosition(pos + mPos);
        eTY.setPosition(iconDEF.getPosition() + Vec2f{0, 10 + 1});
        srtDEF.txt.setPosition(iconDEF.getPosition() + Vec2f{15.f, 0});
        mGW.draw(iconDEF);
        mGW.draw(srtDEF.txt);
        mGW.draw(eTY);

        appendElems(mGW, eTY, mA.elementTypes);
    }

    inline void draw(
    Armor& mA, ssvs::GameWindow& mGW, const Vec2f& mPos, const Vec2f& mCenter)
    {
        srtDEF.set(mA.def);
        commonDraw(mA, mGW, mPos, mCenter);
    }

    inline void draw(Creature& mC, ssvs::GameWindow& mGW, const Vec2f& mPos,
    const Vec2f& mCenter)
    {
        srtDEF.set(mC.armor.def, mC.bonusDEF);
        commonDraw(mC.armor, mGW, mPos, mCenter);
    }
};
}

#endif
