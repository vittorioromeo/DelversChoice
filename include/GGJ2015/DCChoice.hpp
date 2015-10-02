#ifndef GGJ2015_DCCHOICE
#define GGJ2015_DCCHOICE

#include "../GGJ2015/DCCommon.hpp"

namespace ggj
{
struct Choice
{
    GameSession& gameSession;
    SizeT idx;

    inline Choice(GameSession& mGameState, SizeT mIdx)
        : gameSession{mGameState}, idx{mIdx}
    {
    }
    inline virtual ~Choice() {}

    inline virtual void execute() {}
    inline virtual void update(FT) {}
    inline virtual void draw(ssvs::GameWindow&, const Vec2f&, const Vec2f&) {}

    inline virtual std::string getChoiceStr() { return ""; }
};
}

#endif
