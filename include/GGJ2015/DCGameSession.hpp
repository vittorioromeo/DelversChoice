#ifndef GGJ2015_GAMESESSION
#define GGJ2015_GAMESESSION

#include "../GGJ2015/DCCommon.hpp"
#include "../GGJ2015/DCInstantEffect.hpp"

namespace ggj
{
	struct GameSession
	{
		enum class State : int{Playing = 0, Dead = 1, Menu = 2};
		enum class Mode : int{Beginner = 0, Official = 1, Hardcore = 2};

		State state{State::Menu};
		int roomNumber{0};
		Creature player;
		ssvu::UPtr<Choice> choices[Constants::maxChoices];
		ssvu::UPtr<Choice> nextChoices[Constants::maxChoices];
		float timer;

		GameData gd;

		sf::SoundBuffer* currentMusic{nullptr};
		sf::Sound music;

		ItemDrops* currentDrops{nullptr};

		float shake{0}, deathTextTime{0};

		Mode mode{Mode::Official};

		inline void restart()
		{
			music.stop();
			getAssets().soundPlayer.stop();

			// Load gamedata
			std::string modeKey{"beginner"};
			if(mode == Mode::Official) modeKey = "official";
			else if(mode == Mode::Hardcore) modeKey = "hardcore";
			gd = GameData{modeKey};

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

			state = State::Menu;

			currentMusic = getAssets().menu;
			refreshMusic();
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


		inline void refreshChoices()
		{
			for(auto i(0u); i < Constants::maxChoices; ++i)
			{
				if(nextChoices[i] == nullptr) continue;
				choices[i] = std::move(nextChoices[i]);
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

			if(ssvu::getRnd(0, 100) < 50) mX[indices[i++]] = true;

			if(roomNumber < 20) return;
			if(ssvu::getRnd(0, 100) < 45) mX[indices[i++]] = true;

			if(roomNumber < 30) return;
			if(ssvu::getRnd(0, 100) < 40) mX[indices[i++]] = true;

			if(roomNumber < 40) return;
			if(ssvu::getRnd(0, 100) < 35) mX[indices[i++]] = true;
		}

		inline InstantEffect generateInstantEffect(InstantEffect::Stat mStat, InstantEffect::Type mType, float mValue)
		{
			float ieValue;

			switch(mStat)
			{
				case InstantEffect::Stat::SHPS: ieValue = mValue / gd.valueHPS; break;
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
			if(ssvu::getRnd(0, 50) > 25) std::swap(splitPositive, splitNegative);

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
			auto dIE(ssvu::makeUPtr<DropIE>(*this));

			addIEs(*dIE);

			if(ssvu::getRnd(0, 100) < 35)
			{
				addIEs(*dIE);
			}

			// if(ssvu::getRnd(0, 100) < 25) dIE->addIE(generateInstantEffect(mL));

			return dIE;
		}

		inline auto generateDropWeapon()
		{
			auto dr(ssvu::makeUPtr<WeaponDrop>(*this));
			dr->weapon = generateWeapon(gd.getRndDropValue());

			return dr;
		}

		inline auto generateDropArmor()
		{
			auto dr(ssvu::makeUPtr<ArmorDrop>(*this));
			dr->armor = generateArmor(gd.getRndDropValue());

			return dr;
		}

		inline ssvu::UPtr<Drop> generateRndDrop()
		{
			if(ssvu::getRnd(0, 50) > 21)
			{
				return std::move(generateDropIE());
			}
			else
			{
				if(ssvu::getRnd(0, 50) > 19)
					return std::move(generateDropWeapon());
				else
					return std::move(generateDropArmor());
			}
		}

		inline ItemDrops generateDrops()
		{
			ItemDrops result;

			auto i(0u);
			result.drops[i] = std::move(generateRndDrop());

			for(; i < Constants::maxDrops; ++i)
			{
				if(ssvu::getRnd(0, 50) > 20) continue;

				result.drops[i] = std::move(generateRndDrop());
			}

			return result;
		}

		inline Weapon generateWeapon(float mValue)
		{
			Weapon result;

			result.name = "TODO";
			result.atk = gd.getATK(mValue);
			generateRndElements(result.strongAgainst);
			generateRndElements(result.weakAgainst);
			result.type = static_cast<Weapon::Type>(ssvu::getRnd(0, 3));

			return result;
		}

		inline Armor generateArmor(float mValue)
		{
			Armor result;

			result.name = "TODO";
			result.def = gd.getDEF(mValue) * 0.8f;
			generateRndElements(result.elementTypes);

			return result;
		}

		inline Creature generateCreature(bool mEnemy)
		{
			auto valueTotal(gd.getRndValue(mEnemy));



			// TODO: cleanup, ssvu, etc
			std::vector<float> bucket;
			for(auto i(0u); i < 2; ++i) bucket.emplace_back(ssvu::getRndR(0, 3.f));
			bucket.emplace_back(0.f);
			bucket.emplace_back(3.f);
			ssvu::sort(bucket);
			ssvu::lo() << bucket << "\n";

			auto split1(bucket[1] - bucket[0]);
			auto split2(bucket[2] - bucket[1]);
			auto split3(bucket[3] - bucket[2]);

			auto valueSplit1(valueTotal * split1 / 3.f);
			auto valueSplit2(valueTotal * split2 / 3.f);
			auto valueSplit3(valueTotal * split3 / 3.f);

			ssvu::lo()	<< "valueTotal: " << valueTotal << "\n"
						<< "valueSplit1: " << valueSplit1 << "\n"
						<< "valueSplit2: " << valueSplit2 << "\n"
						<< "valueSplit3: " << valueSplit3 << "\n\n";

			Creature result;

			// result.name = getGen().generateCreatureName();
			result.name = "TODO";
			result.armor = generateArmor(valueSplit1);
			result.weapon = generateWeapon(valueSplit2);
			result.hps = gd.getHPS(valueSplit3);

			return result;
		}

		inline ssvu::UPtr<Choice> generateChoiceCreature(int mIdx)
		{
			auto choice(ssvu::makeUPtr<ChoiceCreature>(*this, mIdx));
			choice->creature = generateCreature(true);
			return std::move(choice);
		}

		inline ssvu::UPtr<Choice> generateChoiceSingleDrop(int mIdx)
		{
			auto choice(ssvu::makeUPtr<ChoiceSingleDrop>(*this, mIdx));
			choice->drop = generateRndDrop();
			return std::move(choice);
		}

		inline ssvu::UPtr<Choice> generateChoiceMultipleDrop(int mIdx)
		{
			auto choice(ssvu::makeUPtr<ChoiceItemDrop>(*this, mIdx));
			choice->itemDrops = generateDrops();
			return std::move(choice);
		}

		// TODO: test, cleanup, to ssvu, remove in gen?
		template<typename T> inline auto& weightedChance(T& mC)
		{
			float weightTotal{0.f};
			for(auto i(0u); i < mC.size(); ++i) weightTotal += mC[i].first;

			auto r(ssvu::getRndR(0, weightTotal));
			for(auto i(0u); i < mC.size(); ++i)
			{
				if(r < mC[i].first) return mC[i].second;
				r -= mC[i].second;
			}

			SSVU_UNREACHABLE();
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

			std::vector<std::pair<float, int>> v
			{
				{gd.choiceChanceCreature, 0},
				{gd.choiceChanceSingleDrop, 1},
				{gd.choiceChanceMultipleDrop, 2}
			};

			for(int i{0}; i < choiceCount; ++i)
			{
				auto idx(indices[i]);
				auto type(weightedChance(v));

				switch(type)
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
			nextChoices[mIdx] = ssvu::fwd<T>(mX);
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

		inline void die()
		{
			music.stop();
			getAssets().soundPlayer.play(*getAssets().lose);
			shake = 250;
			deathTextTime = 255;
			state = GameSession::State::Dead;
		}
	};
}

#endif
