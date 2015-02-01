#include "../GGJ2015/Common.hpp"
#include "../GGJ2015/Boilerplate.hpp"

#include "../GGJ2015/DCCommon.hpp"
#include "../GGJ2015/DCAssetLoader.hpp"
#include "../GGJ2015/DCAssets.hpp"
#include "../GGJ2015/DCEventLog.hpp"
#include "../GGJ2015/DCWeapon.hpp"
#include "../GGJ2015/DCArmor.hpp"
#include "../GGJ2015/DCCalculations.hpp"
#include "../GGJ2015/DCCreature.hpp"
#include "../GGJ2015/DCGen.hpp"
#include "../GGJ2015/DCInstantEffect.hpp"
#include "../GGJ2015/DCDrop.hpp"
#include "../GGJ2015/DCElems.hpp"
#include "../GGJ2015/DCStatRichText.hpp"
#include "../GGJ2015/DCWeaponStatsDraw.hpp"
#include "../GGJ2015/DCArmorStatsDraw.hpp"
#include "../GGJ2015/DCCreatureStatsDraw.hpp"
#include "../GGJ2015/DCWeaponDrop.hpp"
#include "../GGJ2015/DCArmorDrop.hpp"
#include "../GGJ2015/DCDropIE.hpp"
#include "../GGJ2015/DCItemDrops.hpp"
#include "../GGJ2015/DCChoice.hpp"
#include "../GGJ2015/DCChoiceAdvance.hpp"
#include "../GGJ2015/DCChoiceCreature.hpp"
#include "../GGJ2015/DCChoiceItemDrop.hpp"
#include "../GGJ2015/DCChoiceSingleDrop.hpp"
#include "../GGJ2015/DCGameSession.hpp"
#include "../GGJ2015/DCChoiceAdvance.inl"
#include "../GGJ2015/DCChoiceCreature.inl"
#include "../GGJ2015/DCChoiceItemDrop.inl"
#include "../GGJ2015/DCChoiceSingleDrop.inl"
#include "../GGJ2015/DCCreature.inl"
#include "../GGJ2015/DCInstantEffect.inl"
#include "../GGJ2015/DCSlotChoice.hpp"
#include "../GGJ2015/DCGameApp.hpp"

// TODO: ssvu::Maybe<T> / Uncertain<T>?
// TODO: better resource caching system in SSVS
// TODO: load resources from folder, not json?
// TODO: aspect ratio resizing
// TODO: game state virtual funcs
// TODO: save profile

int main()
{
	SSVUT_RUN();
/*
	int n = 0;

	auto x = [&n](int xx){ n += xx * 1; ssvu::lo() << "0\n"; };
	auto y = [&n](int xx){ n += xx * 2; ssvu::lo() << "1\n"; };
	auto z = [&n](int xx){ n += xx * 3; ssvu::lo() << "2\n"; };

	auto tpl = std::make_tuple(x, y, z);

	using XType = void (decltype(x)::*)(int);

	std::vector<XType> f;
	f.emplace_back(x);
	f.emplace_back(reinterpret_cast<XType&>(y));
	f.emplace_back(reinterpret_cast<XType&>(z));

	ssvu::tplForIdx([&f](auto i, auto& xx){ (xx.*(f[i]))(2); }, tpl);

	// 0 + 2 * 1 + 2 * 2 * 2 * 3
	// 0 + 2 + 4 + 6
	// 12

	ssvu::lo() << n << "\n";

return 0;
*/
	Boilerplate::AppRunner<ggj::GameApp>{"Delver's choice - GGJ2015 - RC7", 320, 240};
	return 0;
}

