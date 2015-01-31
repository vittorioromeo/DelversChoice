#ifndef GGJ2015_DCCHOICESINGLEDROP
#define GGJ2015_DCCHOICESINGLEDROP

#include "../GGJ2015/DCCommon.hpp"

namespace ggj
{
	struct ChoiceSingleDrop : public Choice
	{
		ssvu::UPtr<Drop> drop{nullptr};

		ChoiceSingleDrop(GameSession& mGS, SizeT mIdx);

		void execute() override;
		void update(FT) override;
		inline void draw(ssvs::GameWindow&, const Vec2f&, const Vec2f&) override;

		inline std::string getChoiceStr() override { return "Pickup"; }
	};
}

#endif
