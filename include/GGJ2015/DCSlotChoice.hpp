#ifndef GGJ2015_SLOTCHOICE
#define GGJ2015_SLOTCHOICE

#include "../GGJ2015/DCCommon.hpp"

namespace ggj
{
	struct SlotChoice
	{
		sf::RectangleShape shape;
		sf::Sprite sprite;
		ssvs::BitmapText txtNum;
		ssvs::BitmapText txtStr;
		int choice;

		static constexpr float step{300.f / 4.f};

		inline SlotChoice(int mChoice) : txtNum{*getAssets().fontObBig, ssvu::toStr(mChoice + 1)},
			txtStr{mkTxtOBSmall()}, choice{mChoice}
		{
			shape.setSize(Vec2f{step, 130.f});
			shape.setFillColor(sf::Color::Red);
			shape.setPosition(Vec2f{10 + step * mChoice, 40});

			sprite.setTexture(*getAssets().slotChoice);
			sprite.setPosition(Vec2f{10 + step * mChoice, 40});

			ssvs::setOrigin(txtNum, ssvs::getLocalCenter);
			txtNum.setPosition(Vec2f{10 + step * mChoice + (step / 2.f), 40 + 105});
		}

		inline void update()
		{
			ssvs::setOrigin(txtStr, ssvs::getLocalCenter);
			txtStr.setPosition(Vec2f{10 + step * choice + (step / 2.f), 40 + 120});
		}

		inline Vec2f getCenter()
		{
			return Vec2f{10 + (step * choice) + (step / 2.f), 40 + 130.f / 2.f};
		}

		inline void drawInCenter(ssvs::GameWindow& mGW, const sf::Texture& mX)
		{
			sf::Sprite s;
			s.setTexture(mX);
			ssvs::setOrigin(s, ssvs::getLocalCenter);
			s.setPosition(getCenter());
			mGW.draw(s);
		}
	};
}

#endif
