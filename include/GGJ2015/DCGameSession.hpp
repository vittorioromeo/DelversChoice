#ifndef GGJ2015_GAMESESSION
#define GGJ2015_GAMESESSION

#include "../GGJ2015/DCCommon.hpp"

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
		float difficulty{1.f};
		float rndMultiplier{1.2f};

		sf::SoundBuffer* currentMusic{nullptr};
		sf::Sound music;

		ItemDrops* currentDrops{nullptr};

		float shake{0}, deathTextTime{0};
		float difficultyInc{0.03f};

		Mode mode{Mode::Official};
		bool timerEnabled{true};

		inline void sustain()
		{
			if(player.isDead()) return;

			float x(1.f + (roomNumber * 1.5f / difficulty));
			ssvu::clampMax(x, 20);

			eventLo() << "You drain " << static_cast<int>(x) << " HPS defeating the enemy\n";
			player.hps += x;
		}

		inline void restart()
		{
			music.stop();
			getAssets().soundPlayer.stop();

			if(mode == Mode::Official || mode == Mode::Beginner) { difficulty = 1.f; difficultyInc = 0.038f; }
			if(mode == Mode::Hardcore) { difficulty = 1.f; difficultyInc = 0.087f; }

			timerEnabled = (mode != Mode::Beginner);

			state = State::Playing;
			roomNumber = 0;
			shake = deathTextTime = 0.f;
			for(auto& c : choices) c.release();
			for(auto& c : nextChoices) c.release();

			Weapon startingWeapon;
			startingWeapon.atk = 5;
			startingWeapon.name = "Starting weapon";
			player.bonusATK = 1;

			Armor startingArmor;
			startingArmor.def = 2;
			startingArmor.name = "Starting armor";
			player.bonusDEF = 1;

			player.name = "Player";
			player.hps = 150;
			player.weapon = startingWeapon;
			player.armor = startingArmor;

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

		inline void generateRndElements(int mL, ElementBitset& mX)
		{
			auto d(static_cast<int>(mL * difficulty));

			if(roomNumber < 10) return;

			auto i(0u);
			auto indices(mkShuffledVector<int>(0, 1, 2, 3));

			if(ssvu::getRnd(0, 100) < 50) mX[indices[i++]] = true;

			if(d < 20) return;
			if(ssvu::getRnd(0, 100) < 45) mX[indices[i++]] = true;

			if(d < 30) return;
			if(ssvu::getRnd(0, 100) < 40) mX[indices[i++]] = true;

			if(d < 40) return;
			if(ssvu::getRnd(0, 100) < 35) mX[indices[i++]] = true;
		}

		inline int getRndStat(int mL, float, float)
		{
			auto d(static_cast<int>(((mL * 0.8f) + 4) * difficulty));

			return ssvu::getClampedMin(ssvu::getRnd((int)(d * 0.65f), (int)(d * 1.55f)), 0);
//			return ssvu::getClampedMin(1, d + ssvu::getRnd(static_cast<int>((mMultMin * d) * rndMultiplier), static_cast<int>((mMultMax * d) * rndMultiplier)));
		}

		inline InstantEffect generateInstantEffect(InstantEffect::Stat mStat, InstantEffect::Type mType, int mL)
		{
			float val(ssvu::getClampedMin((mL / 8) + ssvu::getRnd(0, 3 + (mL / 12)), 1));
			if(mStat == InstantEffect::Stat::SHPS) val = mL * (10 + ssvu::getRnd(-2, 3));

			return {mType, mStat, val};
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

		inline auto addIEs(int mL, DropIE& dIE)
		{
			auto ss(getShuffledStats());

			dIE.addIE(generateInstantEffect(ss[0], InstantEffect::Type::Add, mL));
			dIE.addIE(generateInstantEffect(ss[1], InstantEffect::Type::Sub, mL));
		}


		inline auto generateDropIE(int mL)
		{
			auto dIE(ssvu::makeUPtr<DropIE>(*this));

			addIEs(mL, *dIE);

			if(ssvu::getRnd(0, 100) < ssvu::getClampedMax(mL, 35))
			{
				addIEs(mL, *dIE);
			}

			// if(ssvu::getRnd(0, 100) < 25) dIE->addIE(generateInstantEffect(mL));

			return dIE;
		}

		inline auto generateDropWeapon(int mL)
		{
			auto dr(ssvu::makeUPtr<WeaponDrop>(*this));
			dr->weapon = generateWeapon(mL);

			return dr;
		}

		inline auto generateDropArmor(int mL)
		{
			auto dr(ssvu::makeUPtr<ArmorDrop>(*this));
			dr->armor = generateArmor(mL);

			return dr;
		}

		inline ssvu::UPtr<Drop> generateRndDrop(int mL)
		{
			if(ssvu::getRnd(0, 50) > 21)
			{
				return std::move(generateDropIE(mL));
			}
			else
			{
				if(ssvu::getRnd(0, 50) > 19)
					return std::move(generateDropWeapon(mL));
				else
					return std::move(generateDropArmor(mL));
			}
		}

		inline ItemDrops generateDrops(int mL)
		{
//			auto d(static_cast<int>(mL * difficultyMultiplier));

			ItemDrops result;

			auto i(0u);
			result.drops[i] = std::move(generateRndDrop(mL));

			for(; i < Constants::maxDrops; ++i)
			{
				if(ssvu::getRnd(0, 50) > 20) continue;

				result.drops[i] = std::move(generateRndDrop(mL));
			}

			return result;
		}

		inline Weapon generateWeapon(int mL)
		{
			auto d(static_cast<int>(mL * difficulty));

			Weapon result;

			result.name = "Generated name TODO (lvl: " + ssvu::toStr(d) + ")";
			result.atk = getRndStat(mL, 0.5f, 1.8f) + 1;
			generateRndElements(mL, result.strongAgainst);
			generateRndElements(mL, result.weakAgainst);
			result.type = static_cast<Weapon::Type>(ssvu::getRnd(0, 3));

			return result;
		}

		inline Armor generateArmor(int mL)
		{
			auto d(static_cast<int>(mL * difficulty));

			Armor result;

			result.name = "Generated name TODO (lvl: " + ssvu::toStr(d) + ")";
			result.def = getRndStat(mL, 0.5f, 1.8f) * 0.7f;
			generateRndElements(mL, result.elementTypes);

			return result;
		}

		inline Creature generateCreature(int mL)
		{
			auto d(static_cast<int>(mL * difficulty));

			Creature result;

			result.name = getGen().generateCreatureName();
			result.armor = generateArmor(ssvu::getClampedMin(mL * 0.69f + difficulty - 1, 1));
			result.weapon = generateWeapon(mL - 1);
			result.hps = d * 5 + ssvu::getRnd(0, d * 3);

			return result;
		}

		inline ssvu::UPtr<Choice> generateChoiceCreature(int mIdx, int mL)
		{
			auto choice(ssvu::makeUPtr<ChoiceCreature>(*this, mIdx));
			choice->creature = generateCreature((mL + difficulty + (roomNumber / 10)) * difficulty);
			return std::move(choice);
		}

		inline ssvu::UPtr<Choice> generateChoiceSingleDrop(int mIdx, int mL)
		{
			auto choice(ssvu::makeUPtr<ChoiceSingleDrop>(*this, mIdx));
			choice->drop = generateRndDrop(mL);
			return std::move(choice);
		}

		inline ssvu::UPtr<Choice> generateChoiceMultipleDrop(int mIdx, int mL)
		{
			auto choice(ssvu::makeUPtr<ChoiceItemDrop>(*this, mIdx));
			choice->itemDrops = generateDrops(mL);
			return std::move(choice);
		}

		inline void generateChoices()
		{
			auto choiceNumber(2);

			if(roomNumber > 10) choiceNumber = 3;
			else if(roomNumber > 20) choiceNumber = 4;

			auto indices(mkShuffledVector<int>(0, 1, 2, 3));
			for(auto& c : choices) c.release();

			for(int i{0}; i < choiceNumber; ++i)
			{
				auto idx(indices[i]);

				if(ssvu::getRnd(0, 100) > 15)
				{
					choices[idx] = generateChoiceCreature(idx, roomNumber);
				}
				else
				{
					if(ssvu::getRnd(0, 100) > 20)
					{
						choices[idx] = generateChoiceSingleDrop(idx, roomNumber);
					}
					else
					{
						choices[idx] = generateChoiceMultipleDrop(idx, roomNumber);
					}
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

			if(roomNumber < 10)			currentMusic = getAssets().lvl1;
			else if(roomNumber < 20)	currentMusic = getAssets().lvl2;
			else if(roomNumber < 30)	currentMusic = getAssets().lvl3;
			else if(roomNumber < 40)	currentMusic = getAssets().lvl4;

			refreshMusic();

			if(roomNumber % 5 == 0)
			{
				eventLo() << "Increasing difficulty...\n";
				difficulty += difficultyInc;
			}

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
