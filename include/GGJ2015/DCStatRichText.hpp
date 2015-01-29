#ifndef GGJ2015_DCSTATRICHTEXT
#define GGJ2015_DCSTATRICHTEXT

#include "../GGJ2015/DCCommon.hpp"

namespace ggj
{
	struct StatRichText
	{
		ssvs::BitmapTextRich txt{*getAssets().fontObStroked};
		BP::Group* pssExtra;
		BP::Str* psTotal;
		BP::Str* psBase;
		BP::Str* psBonus;
		BP::ClFG* clfgBonus;

		inline StatRichText()
		{
			txt	<< txt.mk<BP::Trk>(-3)
				<< sf::Color::White
				<< txt.mk<BP::Str>(psTotal, "")
				<< txt.mk<BP::Group>(pssExtra);

			(*pssExtra) << sf::Color::White
						<< " ("
						<< pssExtra->mk<BP::Str>(psBase, "")
						<< sf::Color::White
						<< "+"
						<< pssExtra->mk<BP::ClFG>(clfgBonus, sf::Color::Green)
						<< pssExtra->mk<BP::Str>(psBonus, "")
						<< sf::Color::White
						<< ")";

			clfgBonus->setAnimPulse(0.05f, 100);
		}

		inline void update(FT mFT) { txt.update(mFT); }

		inline void set(StatType mX)
		{
			pssExtra->setEnabled(false);

			auto s(ssvu::toStr(mX));
			psTotal->setStr(s);
		}

		inline void set(StatType mBase, StatType mBonus)
		{
			if(mBonus == 0)
			{
				set(mBase);
				return;
			}

			pssExtra->setEnabled(true);

			auto sBase(ssvu::toStr(mBase));
			auto sBonus(ssvu::toStr(mBonus));
			auto sTotal(ssvu::toStr(mBase + mBonus));

			psTotal->setStr(sTotal);
			psBase->setStr(sBase);
			psBonus->setStr(sBonus);
		}
	};
}

#endif
