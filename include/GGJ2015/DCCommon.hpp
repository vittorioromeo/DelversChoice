#ifndef GGJ2015_DCCOMMON
#define GGJ2015_DCCOMMON

#include "../GGJ2015/Common.hpp"
#include "../GGJ2015/Boilerplate.hpp"
#include "../GGJ2015/NewBTR/NewBTR.hpp"

namespace ggj
{
	using sfc = sf::Color;
	namespace BS = ssvs::BS;

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
		static constexpr SizeT maxMenuChoices{5};
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

		static GameData fromFile(const std::string& mMode);

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
		static constexpr const char* profilePath{"Data/profileData.json"};

		std::string name{""};

		int scoreBeginner{0};
		int scoreOfficial{0};
		int scoreHardcore{0};

		int gamesPlayed{0};
		int timePlayed{0};

		static ProfileData fromFile();
		void saveToJson();
	};
}

SSVJ_CNV_OBJ_AUTO(ggj::GameData,
	timerEnabled, timerMax,
	valueHPS, valueATK, valueDEF,
	difficulty, difficultyInc, difficultyEnemyMult,
	rndRangeMultiplier, meanDeviationRatio,
	section1, section2, section3, section4,
	section0ChoiceCount, section1ChoiceCount, section2ChoiceCount, section3ChoiceCount, section4ChoiceCount,
	choiceChanceCreature, choiceChanceSingleDrop, choiceChanceMultipleDrop,
	dropDeviationMult, dropValueMult,
	multipleDropChance,
	dropChanceIE, dropChanceWeapon, dropChanceArmor,
	multipleIEChance)

SSVJ_CNV_OBJ_AUTO(ggj::ProfileData,
	name,
	scoreBeginner, scoreOfficial, scoreHardcore,
	gamesPlayed, timePlayed)

inline ggj::GameData ggj::GameData::fromFile(const std::string& mMode)
{
	return ssvj::fromFile("Data/gameData.json")[mMode].as<GameData>();
}

inline ggj::ProfileData ggj::ProfileData::fromFile()
{
	if(ssvufs::Path{profilePath}.exists<ssvufs::Type::All>())
		return ssvj::fromFile(profilePath).as<ProfileData>();

	return ProfileData{};
}

inline void ggj::ProfileData::saveToJson()
{
	ssvj::Val{*this}.writeToFile(profilePath);
}

#endif
