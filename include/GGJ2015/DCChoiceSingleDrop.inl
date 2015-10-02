#ifndef GGJ2015_DCCHOICESINGLEDROP_INL
#define GGJ2015_DCCHOICESINGLEDROP_INL

#include "../GGJ2015/DCCommon.hpp"

namespace ggj
{
inline ChoiceSingleDrop::ChoiceSingleDrop(GameSession& mGS, SizeT mIdx)
    : Choice{mGS, mIdx}
{
}
inline void ChoiceSingleDrop::execute()
{
    if(drop == nullptr) return;

    drop->apply(gameSession.player);
    gameSession.resetChoiceAt(
    idx, ssvu::mkUPtr<ChoiceAdvance>(gameSession, idx));
}
inline void ChoiceSingleDrop::update(FT mFT)
{
    if(drop == nullptr) return;
    drop->update(mFT);
}
inline void ChoiceSingleDrop::draw(
ssvs::GameWindow& mGW, const Vec2f& mPos, const Vec2f& mCenter)
{
    if(drop == nullptr) return;
    drop->draw(mGW, mPos, mCenter);
}
}

#endif
