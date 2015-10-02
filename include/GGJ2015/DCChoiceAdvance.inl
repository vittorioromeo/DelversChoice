#ifndef GGJ2015_DCCHOICEADVANCE_INL
#define GGJ2015_DCCHOICEADVANCE_INL

#include "../GGJ2015/DCCommon.hpp"

namespace ggj
{
inline void ChoiceAdvance::execute() { gameSession.advance(); }
inline void ChoiceAdvance::draw(
ssvs::GameWindow& mGW, const Vec2f&, const Vec2f& mCenter)
{
    advanceSprite.setPosition(mCenter);
    mGW.draw(advanceSprite);
}
}

#endif
