#ifndef GGJ2015_DCCHOICEITEMDROP
#define GGJ2015_DCCHOICEITEMDROP

#include "../GGJ2015/DCCommon.hpp"

namespace ggj
{
    struct ChoiceItemDrop final : public Choice
    {
        sf::Sprite drops;
        ItemDrops itemDrops;

        ChoiceItemDrop(GameSession& mGS, SizeT mIdx);

        inline void execute() override;
        inline void draw(
            ssvs::GameWindow&, const Vec2f&, const Vec2f&) override;

        inline std::string getChoiceStr() override { return "Collect"; }
    };
}

#endif
