#ifndef GGJ2015_DCDROP
#define GGJ2015_DCDROP

#include "../GGJ2015/DCCommon.hpp"

namespace ggj
{
struct Drop
{
    GameSession& gameSession;
    sf::Sprite card;

    inline Drop(GameSession& mGameSession) : gameSession{mGameSession}
    {
        card.setTexture(*getAssets().itemCard);
        ssvs::setOrigin(card, ssvs::getLocalCenter);
    }

    inline virtual ~Drop() {}
    inline virtual void apply(Creature&) {}

    inline virtual void update(FT) {}
    inline virtual void draw(
    ssvs::GameWindow& mGW, const Vec2f&, const Vec2f& mCenter)
    {
        card.setPosition(mCenter + Vec2f{0, -20.f});
        mGW.draw(card);
    }
};
}

#endif
