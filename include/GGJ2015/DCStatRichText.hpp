#ifndef GGJ2015_DCSTATRICHTEXT
#define GGJ2015_DCSTATRICHTEXT

#include "../GGJ2015/DCCommon.hpp"

namespace ggj
{
	struct StatRichText
	{
		ssvs::BitmapTextRich txt{*getAssets().fontObStroked};

		BS::Chunk* p1;
		BS::Chunk* p2;

		BS::Chunk* psTotal;
		BS::Chunk* psBase;
		BS::Chunk* psBonus;

		inline StatRichText()
		{
			sfc gray{100, 100, 100, 255};

			txt.eff<BS::Tracking>(-3).mk(p1).mk(p2);
			p1->eff(sfc::White).in(psTotal);
			p2->eff(gray).in(" (").eff(sfc::White).in(psBase).eff(gray).in("+").eff<BS::PulseDef>(sfc::Green).in(psBonus).eff(gray).in(")");
		}

		inline void update(FT mFT) { txt.update(mFT); }

		inline void set(StatType mX)
		{
			p2->setEnabled(false);

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

			p2->setEnabled(true);

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
