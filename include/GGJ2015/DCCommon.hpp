#ifndef GGJ2015_DCCOMMON
#define GGJ2015_DCCOMMON

#include "../GGJ2015/Common.hpp"
#include "../GGJ2015/Boilerplate.hpp"

namespace ggj
{
	namespace BP = ssvs::BTRP;
	using sfc = sf::Color;

	struct GameSession;

	using StatType = int;
	using HPS = StatType;
	using ATK = StatType;
	using DEF = StatType;

	struct Constants
	{
		static constexpr SizeT elementCount{4};
		static constexpr SizeT maxChoices{4};
		static constexpr SizeT maxDrops{3};
		static constexpr float bonusMultiplier{2.5f};
		static constexpr float malusMultiplier{0.8f};
	};

	using ElementBitset = std::bitset<Constants::elementCount>;

	template<typename TArg, typename... TArgs> inline auto mkShuffledVector(TArg&& mArg, TArgs&&... mArgs)
	{
		std::vector<TArg> result;
		result.reserve(1 + sizeof...(TArgs));
		result.emplace_back(FWD(mArg));
		ssvu::forArgs([&result](auto&& mX){ result.emplace_back(FWD(mX)); }, FWD(mArgs)...);
		ssvu::shuffle(result);
		return result;
	}

	inline auto getStatDisplayStr(StatType mBase, StatType mBonus)
	{
		return ssvu::toStr(mBase + mBonus) + " (" + ssvu::toStr(mBase) + "+" + ssvu::toStr(mBonus) + ")";
	}

	struct GameData
	{
		bool timerEnabled;
		int timerMax;

		float valueHPS;
		float valueATK;
		float valueDEF;

		float difficulty;
		float difficultyInc;
		float difficultyEnemyMult;
		float rndRangeMultiplier;
		float meanDeviationRatio;

		int section1;
		int section2;
		int section3;
		int section4;

		int section0ChoiceCount;
		int section1ChoiceCount;
		int section2ChoiceCount;
		int section3ChoiceCount;
		int section4ChoiceCount;

		float choiceChanceCreature;
		float choiceChanceSingleDrop;
		float choiceChanceMultipleDrop;

		float dropDeviationMult;
		float dropValueMult;

		float multipleDropChance;

		float dropChanceIE;
		float dropChanceWeapon;
		float dropChanceArmor;

		float multipleIEChance;

		inline GameData() { }
		inline GameData(const std::string& mMode)
		{
			using namespace ssvj;

			auto jv(Val::fromFile("Data/gameData.json"));
			const auto& jObj(jv[mMode]);

			#define GGJ_LFJ(mType, mName) mName = jObj[SSVPP_TOSTR(mName)].as<mType>()

			GGJ_LFJ(bool, timerEnabled);
			GGJ_LFJ(int, timerMax);

			GGJ_LFJ(float, valueHPS);
			GGJ_LFJ(float, valueATK);
			GGJ_LFJ(float, valueDEF);

			GGJ_LFJ(float, difficulty);
			GGJ_LFJ(float, difficultyInc);
			GGJ_LFJ(float, difficultyEnemyMult);
			GGJ_LFJ(float, rndRangeMultiplier);
			GGJ_LFJ(float, meanDeviationRatio);

			GGJ_LFJ(int, section1);
			GGJ_LFJ(int, section2);
			GGJ_LFJ(int, section3);
			GGJ_LFJ(int, section4);

			GGJ_LFJ(int, section0ChoiceCount);
			GGJ_LFJ(int, section1ChoiceCount);
			GGJ_LFJ(int, section2ChoiceCount);
			GGJ_LFJ(int, section3ChoiceCount);
			GGJ_LFJ(int, section4ChoiceCount);

			GGJ_LFJ(float, choiceChanceCreature);
			GGJ_LFJ(float, choiceChanceSingleDrop);
			GGJ_LFJ(float, choiceChanceMultipleDrop);

			GGJ_LFJ(float, dropDeviationMult);
			GGJ_LFJ(float, dropValueMult);

			GGJ_LFJ(float, multipleDropChance);

			GGJ_LFJ(float, dropChanceIE);
			GGJ_LFJ(float, dropChanceWeapon);
			GGJ_LFJ(float, dropChanceArmor);

			GGJ_LFJ(float, multipleIEChance);

			#undef GGJ_LFJ
		}

		inline float getRndR(float mMean, float mDeviation) noexcept
		{
			return ssvu::getRndRNormal(mMean, mDeviation);
		}

		inline float getRndValue(bool mEnemy)
		{
			auto mean(difficulty);
			if(mEnemy) mean *= difficultyEnemyMult;
			return getRndR(mean, mean * meanDeviationRatio);
		}

		inline float getRndDropValue()
		{
			auto mean(difficulty);
			return getRndR(mean, mean * (meanDeviationRatio * dropDeviationMult)) * dropValueMult;
		}

		inline float getHPS(float mValue) { return mValue / valueHPS; }
		inline float getATK(float mValue) { return mValue / valueATK; }
		inline float getDEF(float mValue) { return mValue / valueDEF; }
	};

	struct ProfileData
	{
		constexpr static const char* profilePath{"Data/profileData.json"};

		std::string name;

		int scoreBeginner;
		int scoreOfficial;
		int scoreHardcore;

		int gamesPlayed;
		int timePlayed;

		inline ProfileData()
		{
			using namespace ssvj;

			if(!ssvufs::Path{profilePath}.exists<ssvufs::Type::All>())
			{
				Val newProfile{Obj{}};
				/*newProfile["scoreBeginner"] = 0;
				newProfile["scoreOfficial"] = 0;
				newProfile["scoreHardcore"] = 0;

				newProfile["gamesPlayed"] = 0;
				newProfile["timePlayed"] = 0;*/

				newProfile.writeToFile(profilePath);
			}

			auto jv(Val::fromFile(profilePath));

			#define GGJ_LFJ(mName, mDef) mName = jv.getIfHas<decltype(mName)>(SSVPP_TOSTR(mName), decltype(mName){mDef})

			GGJ_LFJ(name, "unnamed");

			GGJ_LFJ(scoreBeginner, 0);
			GGJ_LFJ(scoreOfficial, 0);
			GGJ_LFJ(scoreHardcore, 0);

			GGJ_LFJ(gamesPlayed, 0);
			GGJ_LFJ(timePlayed, 0);

			#undef GGJ_LFJ
		}

		inline void saveToJson()
		{
			using namespace ssvj;

			#define GGJ_LTJ(mName) jv[SSVPP_TOSTR(mName)] = mName;

			auto jv(Val::fromFile(profilePath));

			GGJ_LTJ(name);

			GGJ_LTJ(scoreBeginner);
			GGJ_LTJ(scoreOfficial);
			GGJ_LTJ(scoreHardcore);

			GGJ_LTJ(gamesPlayed);
			GGJ_LTJ(timePlayed);

			jv.writeToFile(profilePath);

			#undef GGJ_LTJ
		}
	};
}

#endif
