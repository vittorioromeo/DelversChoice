#ifndef GGJ2015_DCCHOICEADVANCE
#define GGJ2015_DCCHOICEADVANCE

#include "../GGJ2015/DCCommon.hpp"

namespace ggj
{
struct ChoiceAdvance : public Choice
{
    sf::Sprite advanceSprite;

    inline ChoiceAdvance(GameSession& mGameState, SizeT mIdx)
        : Choice{mGameState, mIdx}
    {
        advanceSprite.setTexture(*getAssets().advance);
        ssvs::setOrigin(advanceSprite, ssvs::getLocalCenter);
    }

    inline void execute() override;
    inline void draw(ssvs::GameWindow&, const Vec2f&, const Vec2f&) override;

    inline std::string getChoiceStr() override { return "Forward"; }
};
}

#endif
