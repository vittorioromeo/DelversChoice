#ifndef GGJ2015_DCGEN
#define GGJ2015_DCGEN

#include "../GGJ2015/DCCommon.hpp"
#include "../GGJ2015/DCAssets.hpp"

namespace ggj
{
	struct CreatureFlavor
	{
		const sf::Texture& texture;
		const std::string name;

		template<typename T> inline CreatureFlavor(const sf::Texture& mTexture, T&& mName) : texture{mTexture}, name{FWD(mName)} { }
	};

	namespace Impl
	{
		inline const auto& getCreatureFlavors()
		{
			static std::vector<CreatureFlavor> result
			{
				{*getAssets().enemy, "Test enemy"}
			};

			return result;
		}

		inline const auto& getCreatureFlavorsChances()
		{
			static auto result(mkWeightedChance
			(
				1.f
			));

			return result;
		}
	}

	inline const auto& getRndCreatureFlavor()
	{
		return Impl::getCreatureFlavors()[Impl::getCreatureFlavorsChances().get()];
	}

	namespace Impl
	{
		struct NameGenData
		{
			float chance;
			std::string str;

			inline NameGenData(float mChance, const std::string& mStr) : chance{mChance}, str{mStr} { }
		};

		// TODO: ?
		struct Gen
		{
			inline const auto& getWeapons()
			{
				static std::vector<NameGenData> result
				{
					{1.0f,		"Sword"},
					{1.0f,		"Spear"},
					{1.0f,		"Staff"},
					{1.0f,		"Gauntlet"},
					{1.0f,		"Wand"},
					{0.8f,		"Greatsword"},
					{0.8f,		"Claymore"},
					{0.7f,		"Magical sword"},
					{0.7f,		"Enchanted gauntlets"},
					{0.5f,		"Greatstaff"},
				};

				return result;
			}

			inline const auto& getItemModifiers()
			{
				static std::vector<NameGenData> result
				{
					{1.0f,		"Rusty"},
					{1.0f,		"Damaged"},
					{1.0f,		"Dented"},
					{1.0f,		"Regular"},
					{0.8f,		"Powerful"},
					{0.8f,		"Intense"},
					{0.8f,		"Heavy"},
					{0.7f,		"Incredible"},
					{0.7f,		"Excellent"},
					{0.5f,		"Supreme"},
				};

				return result;
			}

			inline const auto& getCreatures()
			{
				static std::vector<NameGenData> result
				{
					{1.0f,		"Slime"},
					{1.0f,		"Skeleton"},
					{1.0f,		"Dragonkin"},
					{1.0f,		"Giant crab"},
					{0.8f,		"Undead"},
					{0.8f,		"Zombie"},
					{0.8f,		"Dragon"},
					{0.7f,		"Ghost"},
					{0.7f,		"Bloodkin"},
					{0.5f,		"Scolarship"},
				};

				return result;
			}

			inline const auto& getCreatureModifier()
			{
				static std::vector<NameGenData> result
				{
					{1.0f,		"Injured"},
					{1.0f,		"Diseased"},
					{1.0f,		"Enraged"},
					{1.0f,		"Powerful"},
					{0.8f,		"Undead"},
					{0.8f,		"Magical"},
					{0.8f,		"Enchanted"},
					{0.7f,		"Phantasm"},
					{0.7f,		"Bloodthirsty"},
					{0.5f,		"Ravaging"},
				};

				return result;
			}

			template<typename T> inline const auto& getR(const T& mX)
			{
				float weightSum{0.f};
				for(const auto& x : mX) weightSum += x.chance;
				auto r(ssvu::getRndR(0.f, weightSum));
				auto t(0.f);

				for(const auto& x : mX)
				{
					t += x.chance;
					if(t > r) return x.str;
				}

				return mX[ssvu::getRndI(0ul, mX.size())].str;
			}

			template<typename T> inline void whileChance(int mChance, const T& mFn)
			{
				while(ssvu::getRndI(0, 100) < mChance)
				{
					mFn();
					mChance /= 2;
					if(mChance < 2) mChance = 2;
				}
			}

			inline auto generateWeaponName()
			{
				std::string result;
				return result;
			}

			inline auto generateCreatureName()
			{
				std::string result;

				whileChance(25, [this, &result]{ result += getR(getCreatureModifier()) + " "; });
				result += getR(getCreatures());

				return result;
			}
		};
	}

	inline auto& getGen() noexcept { static Impl::Gen result; return result; }
}

#endif
