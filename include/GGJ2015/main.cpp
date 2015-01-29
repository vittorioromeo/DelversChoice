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
// TODO: rich bitmap text
// TODO: game state virtual funcs

int main()
{
	SSVUT_RUN();

	Boilerplate::AppRunner<ggj::GameApp>{"Delver's choice - GGJ2015 - RC7", 320, 240};
	return 0;
}

