#ifndef GGJ2015_GAMESESSION
#define GGJ2015_GAMESESSION

#include "../GGJ2015/DCCommon.hpp"
#include "../GGJ2015/DCInstantEffect.hpp"

namespace ggj
{
	struct GameSession
	{
		//enum class State : int{Playing = 0, Dead = 1, Menu = 2, Settings = 3};
		enum class State : int{Playing = 0, Dead = 1};
		enum class Mode : int{Beginner = 0, Official = 1, Hardcore = 2};
		// enum class SubMenu : int{Main = 0, SelectMode = 1};

		State state{State::Playing};
		//SubMenu subMenu{SubMenu::Main};
		int roomNumber{0};
		Creature player;
		std::array<ssvu::UPtr<Choice>, Constants::maxChoices> choices, nextChoices;
		float timer;

		GameData gd;
		ProfileData pd;

		sf::SoundBuffer* currentMusic{nullptr};
		sf::Sound music;

		ItemDrops* currentDrops{nullptr};

		float shake{0}, deathTextTime{0};

		Mode mode{Mode::Official};

		WeightedChance<> wcDrop, wcChoice;

		inline void restart()
		{
			music.stop();
			getAssets().soundPlayer.stop();

			// Load gamedata
			std::string modeKey{"beginner"};
			if(mode == Mode::Official) modeKey = "official";
			else if(mode == Mode::Hardcore) modeKey = "hardcore";
			gd = GameData::fromFile(modeKey);

			wcDrop = mkWeightedChance
			(
				gd.dropChanceIE,		// 0
				gd.dropChanceWeapon,	// 1
				gd.dropChanceArmor		// 2
			);

			wcChoice = mkWeightedChance
			(
				gd.choiceChanceCreature,		// 0
				gd.choiceChanceSingleDrop,		// 1
				gd.choiceChanceMultipleDrop		// 2
			);

			state = State::Playing;
			roomNumber = 0;
			shake = deathTextTime = 0.f;
			for(auto& c : choices) c.release();
			for(auto& c : nextChoices) c.release();

			// TODO: name from profile
			player = generateCreature(false);
			player.name = "Player";
			player.bonusATK = 4;
			player.bonusDEF = 2;

			advance();
		}

		inline void gotoMenu()
		{
			music.stop();
			getAssets().soundPlayer.stop();
			shake = deathTextTime = 0.f;

			// die();

			currentMusic = getAssets().menu;
			refreshMusic();

			refreshProfileData();
		}

		inline GameSession()
		{
			gotoMenu();
		}

		inline void tryPickupDrop(int mIdx)
		{
			if(!currentDrops->has(mIdx)) return;

			currentDrops->give(mIdx, player);
		}

		inline void startDrops(ItemDrops* mID)
		{
			currentDrops = mID;
		}
		inline void endDrops()
		{
			currentDrops = nullptr;
		}

		inline void refreshProfileData()
		{
			pd = ProfileData::fromFile();
		}


		inline void refreshChoices()
		{
			for(auto i(0u); i < Constants::maxChoices; ++i)
			{
				if(nextChoices[i] == nullptr) continue;
				choices[i] = ssvu::mv(nextChoices[i]);
				nextChoices[i] = nullptr;
			}
		}

		inline void resetTimer()
		{
			if(mode == Mode::Official || mode == Mode::Beginner) timer = ssvu::getSecondsToFT(10);
			else if(mode == Mode::Hardcore) timer = ssvu::getSecondsToFT(6);
		}

		inline void generateRndElements(ElementBitset& mX)
		{
			// TODO: calc
			if(roomNumber < 10) return;

			auto i(0u);
			auto indices(mkShuffledVector<int>(0, 1, 2, 3));

			if(ssvu::getRndI(0, 100) < 40) mX[indices[i++]] = true;

			if(roomNumber < 20) return;
			if(ssvu::getRndI(0, 100) < 35) mX[indices[i++]] = true;

			if(roomNumber < 30) return;
			if(ssvu::getRndI(0, 100) < 30) mX[indices[i++]] = true;

			if(roomNumber < 40) return;
			if(ssvu::getRndI(0, 100) < 25) mX[indices[i++]] = true;
		}

		inline InstantEffect generateInstantEffect(InstantEffect::Stat mStat, InstantEffect::Type mType, float mValue)
		{
			float ieValue(mValue / gd.valueHPS);

			switch(mStat)
			{
				case InstantEffect::Stat::SHPS: /* as initialized */ break;
				case InstantEffect::Stat::SATK: ieValue = mValue / gd.valueATK; break;
				case InstantEffect::Stat::SDEF: ieValue = mValue / gd.valueDEF; break;
			}

			ssvu::clampMin(ieValue, 1);

			InstantEffect result{mType, mStat, ieValue};

			ssvu::lo() << result.getStrType() << result.getStrStat() << ", value: " << mValue << "\n";

			return result;
		}

		inline auto getShuffledStats()
		{
			return mkShuffledVector<InstantEffect::Stat>
			(
				InstantEffect::Stat::SHPS,
				InstantEffect::Stat::SATK,
				InstantEffect::Stat::SDEF
			);
		}

		inline auto addIEs(DropIE& dIE)
		{
			float valueTotal(gd.getRndDropValue());
			float splitPositive(ssvu::getRndRNormal(0.75f, 0.045f));
			float splitNegative(1.f - splitPositive);
			if(ssvu::getRndI(0, 50) > 25) std::swap(splitPositive, splitNegative);

			// ssvu::lo() << "valueTotal: " << valueTotal << "\n";
					   // << "\nsplitPositive: " << valueTotal * splitPositive
					   // << "\nsplitNegative: " << valueTotal * splitNegative << "\n";

			auto ss(getShuffledStats());

			dIE.addIE(generateInstantEffect(ss[0], InstantEffect::Type::Add, valueTotal * splitPositive));
			dIE.addIE(generateInstantEffect(ss[1], InstantEffect::Type::Sub, valueTotal * splitNegative));

			ssvu::lo() << "\n\n";
		}


		inline auto generateDropIE()
		{
			auto dIE(ssvu::mkUPtr<DropIE>(*this));

			addIEs(*dIE);
			if(ssvu::getRndR(0.f, 1.f) < gd.multipleIEChance) addIEs(*dIE);

			return dIE;
		}

		inline auto generateDropWeapon()
		{
			auto dr(ssvu::mkUPtr<WeaponDrop>(*this));
			dr->weapon = generateWeapon(gd.getRndDropValue());

			return dr;
		}

		inline auto generateDropArmor()
		{
			auto dr(ssvu::mkUPtr<ArmorDrop>(*this));
			dr->armor = generateArmor(gd.getRndDropValue());

			return dr;
		}

		inline ssvu::UPtr<Drop> generateRndDrop()
		{
			switch(wcDrop.get())
			{
				case 0: return ssvu::mv(generateDropIE());
				case 1: return ssvu::mv(generateDropWeapon());
				case 2: return ssvu::mv(generateDropArmor());
			}

			SSVU_UNREACHABLE();
		}

		inline ItemDrops generateDrops()
		{
			ItemDrops result;

			auto indices(mkShuffledVector<int>(0, 1, 2));

			result.drops[indices[0]] = ssvu::mv(generateRndDrop());
			if(ssvu::getRndR(0.f, 1.f) < gd.multipleDropChance) result.drops[indices[1]] = ssvu::mv(generateRndDrop());
			if(ssvu::getRndR(0.f, 1.f) < gd.multipleDropChance) result.drops[indices[2]] = ssvu::mv(generateRndDrop());

			return result;
		}

		inline Weapon generateWeapon(float mValue)
		{
			Weapon result;

			result.name = "TODO";
			result.atk = gd.getATK(mValue);
			generateRndElements(result.strongAgainst);
			generateRndElements(result.weakAgainst);
			result.type = ssvu::toEnum<Weapon::Type>(ssvu::getRndI(0, 3));

			return result;
		}

		inline Armor generateArmor(float mValue)
		{
			Armor result;

			result.name = "TODO";
			result.def = gd.getDEF(mValue);
			generateRndElements(result.elementTypes);

			return result;
		}

		inline Creature generateCreature(bool mEnemy)
		{
			auto valueTotal(gd.getRndValue(mEnemy));

			// TODO: cleanup, ssvu, etc
			auto bucket(ssvu::mkArray
			(
				ssvu::getRndR(0, 3.f),
				ssvu::getRndR(0, 3.f),
				0.f,
				3.f
			));

			ssvu::sort(bucket);
			ssvu::lo() << bucket << "\n";

			auto split1(bucket[1] - bucket[0]);
			auto split2(bucket[2] - bucket[1]);
			auto split3(bucket[3] - bucket[2]);

			auto valueSplit1(valueTotal * split1 / 3.f);
			auto valueSplit2(valueTotal * split2 / 3.f);
			auto valueSplit3(valueTotal * split3 / 3.f);

			auto avg((valueSplit1 + valueSplit2 + valueSplit3) / 3.f);

			// Bring numbers closer together
			// The smaller the coefficient, the closer the numbers will be
			valueSplit1 = avg + 0.4f * (valueSplit1 - avg);
			valueSplit2 = avg + 0.4f * (valueSplit2 - avg);
			valueSplit3 = avg + 0.4f * (valueSplit3 - avg);

			ssvu::lo()	<< "valueTotal: " << valueTotal << "\n"
						<< "valueSplit1: " << valueSplit1 << "\n"
						<< "valueSplit2: " << valueSplit2 << "\n"
						<< "valueSplit3: " << valueSplit3 << "\n\n";

			Creature result;

			// result.name = getGen().generateCreatureName();
			// result.name = "TODO";
			result.armor = generateArmor(valueSplit1);
			result.weapon = generateWeapon(valueSplit2);
			result.hps = gd.getHPS(valueSplit3);

			return result;
		}

		inline auto generateChoiceCreature(int mIdx)
		{
			auto choice(ssvu::mkUPtr<ChoiceCreature>(*this, mIdx));
			choice->creature = generateCreature(true);
			return ssvu::mv(choice);
		}

		inline auto generateChoiceSingleDrop(int mIdx)
		{
			auto choice(ssvu::mkUPtr<ChoiceSingleDrop>(*this, mIdx));
			choice->drop = generateRndDrop();
			return ssvu::mv(choice);
		}

		inline auto generateChoiceMultipleDrop(int mIdx)
		{
			auto choice(ssvu::mkUPtr<ChoiceItemDrop>(*this, mIdx));
			return ssvu::mv(choice);
		}




		inline void generateChoices()
		{
			int choiceCount;

			if(roomNumber < gd.section1)		choiceCount = gd.section0ChoiceCount;
			else if(roomNumber < gd.section2)	choiceCount = gd.section1ChoiceCount;
			else if(roomNumber < gd.section3)	choiceCount = gd.section2ChoiceCount;
			else if(roomNumber < gd.section4)	choiceCount = gd.section3ChoiceCount;
			else								choiceCount = gd.section4ChoiceCount;

			auto indices(mkShuffledVector<int>(0, 1, 2, 3));
			for(auto& c : choices) c.release();

			for(int i{0}; i < choiceCount; ++i)
			{
				auto idx(indices[i]);

				switch(wcChoice.get())
				{
					case 0: choices[idx] = generateChoiceCreature(idx);		break;
					case 1: choices[idx] = generateChoiceSingleDrop(idx);	break;
					case 2: choices[idx] = generateChoiceMultipleDrop(idx);	break;
				}
			}
		}

		inline void refreshMusic()
		{
			music.setLoop(true);
			if(music.getBuffer() != currentMusic) music.setBuffer(*currentMusic);
			if(music.getStatus() != sf::Sound::Status::Playing) music.play();
		}

		template<typename T> inline void resetChoiceAt(SizeT mIdx, T&& mX)
		{
			nextChoices[mIdx] = FWD(mX);
		}

		inline void advance()
		{
			++roomNumber;

			if(roomNumber < gd.section1)		currentMusic = getAssets().lvl1;
			else if(roomNumber < gd.section2)	currentMusic = getAssets().lvl2;
			else if(roomNumber < gd.section3)	currentMusic = getAssets().lvl3;
			else if(roomNumber < gd.section4)	currentMusic = getAssets().lvl4;

			refreshMusic();

			// eventLo() << "Increasing difficulty...\n";

			// TODO: gd.advance();
			gd.difficulty += gd.difficultyInc;

			generateChoices();
			resetTimer();
			endDrops();
		}

		inline void updateProfile()
		{
			++pd.gamesPlayed;

			switch(mode)
			{
				case Mode::Beginner: ++pd.scoreBeginner; break;
				case Mode::Official: ++pd.scoreOfficial; break;
				case Mode::Hardcore: ++pd.scoreHardcore; break;
			}

			++pd.gamesPlayed;

			pd.saveToJson();
		}

		inline void die()
		{
			updateProfile();

			music.stop();
			getAssets().soundPlayer.play(*getAssets().lose);
			shake = 250;
			deathTextTime = 255;
			state = GameSession::State::Dead;
		}
	};
}

#endif
