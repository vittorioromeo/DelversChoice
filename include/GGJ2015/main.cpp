#include "../GGJ2015/Common.hpp"
#include "../GGJ2015/Boilerplate.hpp"

// TODO: better resource caching system in SSVS
// TODO: load resources from folder, not json?
// TODO: aspect ratio resizing
// TODO: rich bitmap text
// TODO: game state virtual funcs

#define CACHE_ASSET(mType, mName, mExt) mType* mName{&assetLoader.assetManager.get<mType>(SSVPP_TOSTR(mName) mExt)}

namespace ggj
{
	class GameSession;

	namespace Impl
	{
		struct AssetLoader
		{
			ssvs::AssetManager assetManager;

			inline AssetLoader()
			{
				ssvs::loadAssetsFromJson(assetManager, "Data/", ssvj::Val::fromFile("Data/assets.json"));
			}
		};

		struct Assets
		{
			AssetLoader assetLoader{};

			// Audio players
			ssvs::SoundPlayer soundPlayer;
			ssvs::MusicPlayer musicPlayer;

			// BitmapFonts
			CACHE_ASSET(ssvs::BitmapFont, fontObStroked, "");
			CACHE_ASSET(ssvs::BitmapFont, fontObBig, "");

			// Textures
			CACHE_ASSET(sf::Texture, slotChoice, ".png");
			CACHE_ASSET(sf::Texture, iconHPS, ".png");
			CACHE_ASSET(sf::Texture, iconATK, ".png");
			CACHE_ASSET(sf::Texture, iconDEF, ".png");
			CACHE_ASSET(sf::Texture, drops, ".png");
			CACHE_ASSET(sf::Texture, enemy, ".png");
			CACHE_ASSET(sf::Texture, blocked, ".png");
			CACHE_ASSET(sf::Texture, back, ".png");
			CACHE_ASSET(sf::Texture, dropsModal, ".png");
			CACHE_ASSET(sf::Texture, advance, ".png");
			CACHE_ASSET(sf::Texture, itemCard, ".png");
			CACHE_ASSET(sf::Texture, eFire, ".png");
			CACHE_ASSET(sf::Texture, eWater, ".png");
			CACHE_ASSET(sf::Texture, eEarth, ".png");
			CACHE_ASSET(sf::Texture, eLightning, ".png");
			CACHE_ASSET(sf::Texture, eST, ".png");
			CACHE_ASSET(sf::Texture, eWK, ".png");
			CACHE_ASSET(sf::Texture, eTY, ".png");
			CACHE_ASSET(sf::Texture, equipCard, ".png");
			CACHE_ASSET(sf::Texture, wpnMace, ".png");
			CACHE_ASSET(sf::Texture, wpnSword, ".png");
			CACHE_ASSET(sf::Texture, wpnSpear, ".png");
			CACHE_ASSET(sf::Texture, armDrop, ".png");

			// Sounds
			CACHE_ASSET(sf::SoundBuffer, lvl1, ".wav");
			CACHE_ASSET(sf::SoundBuffer, lvl2, ".wav");
			CACHE_ASSET(sf::SoundBuffer, lvl3, ".wav");
			CACHE_ASSET(sf::SoundBuffer, lvl4, ".wav");
			CACHE_ASSET(sf::SoundBuffer, menu, ".wav");
			CACHE_ASSET(sf::SoundBuffer, powerup, ".wav");
			CACHE_ASSET(sf::SoundBuffer, drop, ".wav");
			CACHE_ASSET(sf::SoundBuffer, grab, ".wav");
			CACHE_ASSET(sf::SoundBuffer, equipArmor, ".wav");
			CACHE_ASSET(sf::SoundBuffer, equipWpn, ".wav");
			CACHE_ASSET(sf::SoundBuffer, lose, ".wav");

			std::vector<sf::SoundBuffer*> swordSnds, maceSnds, spearSnds;

			inline Assets()
			{
				std::vector<std::string> elems{"normal","fire","water","earth","lightning"};

				for(auto& e : elems)
				{
					swordSnds.emplace_back(&assetLoader.assetManager.get<sf::SoundBuffer>("sword/" + e + ".wav"));
					maceSnds.emplace_back(&assetLoader.assetManager.get<sf::SoundBuffer>("mace/" + e + ".wav"));
					spearSnds.emplace_back(&assetLoader.assetManager.get<sf::SoundBuffer>("spear/" + e + ".wav"));
				}

				soundPlayer.setVolume(100.f);
			}
		};
	}

	inline auto& getAssets() noexcept { static Impl::Assets result; return result; }
	inline auto& getEventLogStream() noexcept { static std::stringstream result; return result; }

	inline auto mkTxtOBSmall()	{ ssvs::BitmapText result{*getAssets().fontObStroked};	result.setTracking(-3); return result; }
	inline auto mkTxtOBBig()	{ ssvs::BitmapText result{*getAssets().fontObBig};		result.setTracking(-1); return result; }

	template<typename TArg, typename... TArgs> inline auto mkShuffledVector(TArg&& mArg, TArgs&&... mArgs)
	{
		std::vector<TArg> result;
		result.emplace_back(ssvu::fwd<TArg>(mArg));
		ssvu::forArgs([&result](auto&& mX){ result.emplace_back(ssvu::fwd<decltype(mX)>(mX)); }, ssvu::fwd<TArgs>(mArgs)...);
		ssvu::shuffle(result);
		return result;
	}

	namespace Impl
	{
		struct EventLog
		{
			template<typename T> inline auto operator<<(const T& mX)
			{
				getEventLogStream() << mX;
				ssvu::lo() << mX;
				return EventLog{};
			}
		};
	}

	inline auto eventLo() noexcept { return Impl::EventLog{}; }

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

	struct Weapon
	{
		enum class Type : int {Mace = 0 , Sword = 1, Spear = 2};

		std::string name{"Unarmed"};
		ElementBitset strongAgainst;
		ElementBitset weakAgainst;
		ATK atk{-1};
		Type type{Type::Mace};

		inline auto& getTypeTexture()
		{
			static auto array(ssvu::makeArray
			(
				getAssets().wpnMace,
				getAssets().wpnSword,
				getAssets().wpnSpear
			));

			return *array[static_cast<int>(type)];
		}

		inline auto& getTypeSoundBufferVec()
		{
			static auto array(ssvu::makeArray
			(
				getAssets().maceSnds,
				getAssets().swordSnds,
				getAssets().spearSnds
			));

			return array[static_cast<int>(type)];
		}

		inline void playAttackSounds()
		{
			auto& vec(getTypeSoundBufferVec());

			// Normal
			if(strongAgainst.none())
			{
				getAssets().soundPlayer.play(*vec[0]);
			}
			else
			{
				for(auto i(0u); i < Constants::elementCount; ++i)
				{
					if(strongAgainst[i]) getAssets().soundPlayer.play(*vec[i + 1]);
				}
			}
		}
	};

	struct Armor
	{
		std::string name{"Unarmored"};
		ElementBitset elementTypes;
		DEF def{-1};
	};

	struct Calculations
	{
		inline static bool isWeaponStrongAgainst(const Weapon& mW, const Armor& mA)
		{
			return (mW.strongAgainst & mA.elementTypes).any();
		}

		inline static bool isWeaponWeakAgainst(const Weapon& mW, const Armor& mA)
		{
			return (mW.weakAgainst & mA.elementTypes).any();
		}

		inline static auto getWeaponDamageAgainst(const Weapon& mW, const Armor& mA, ATK mBonusATK, DEF mBonusDEF)
		{
			auto result((mW.atk + mBonusATK) - (mA.def + mBonusDEF));
			if(isWeaponStrongAgainst(mW, mA)) result *= Constants::bonusMultiplier;
			if(isWeaponWeakAgainst(mW, mA)) result *= Constants::malusMultiplier;
			return ssvu::getClampedMin(result, 0);
		}

		inline static bool canWeaponDamage(const Weapon& mW, const Armor& mA, ATK mBonusATK, DEF mBonusDEF)
		{
			return getWeaponDamageAgainst(mW, mA, mBonusATK, mBonusDEF) > 0;
		}
	};

	struct Creature
	{
		std::string name{"Unnamed"};
		Weapon weapon;
		Armor armor;
		HPS hps{-1};

		ATK bonusATK{0};
		DEF bonusDEF{0};

		inline void attackOnce(Creature& mX)
		{
			auto dmg(Calculations::getWeaponDamageAgainst(weapon, mX.armor, bonusATK, mX.bonusDEF));
			mX.hps -= dmg;
		}

		void checkBurns(GameSession& mGameSession);

		inline void fight(Creature& mX)
		{
			eventLo() << name << " engages " << mX.name << "!\n";
			auto hpsBefore(hps);
			auto xHPSBefore(mX.hps);

			while(true)
			{
				attackOnce(mX);
				if(mX.isDead()) break;

				mX.attackOnce(*this);
				if(isDead()) break;
			}

			if(isDead())
				eventLo() << mX.name << " wins. HPS " << xHPSBefore << " -> " << mX.hps << "!\n";
			else
				eventLo() << name << " wins. HPS " << hpsBefore << " -> " << hps << "!\n";
		}

		inline bool canDamage(Creature& mX) const noexcept
		{
			return Calculations::canWeaponDamage(weapon, mX.armor, bonusATK, mX.bonusDEF);
		}

		inline bool isDead() const noexcept { return hps <= 0; }

		inline std::string getLogStr() const
		{
			std::string result;

			result += "HPS: " + ssvu::toStr(hps) + ", ";
			result += "ATK: " + ssvu::toStr(weapon.atk) + ", ";
			result += "DEF: " + ssvu::toStr(armor.def) + ", ";
			result += "Str: " + ssvu::toStr(weapon.strongAgainst) + ", ";
			result += "Wkk: " + ssvu::toStr(weapon.weakAgainst);

			return result;
		}
	};

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

				return mX[ssvu::getRnd(0ul, mX.size())].str;
			}

			template<typename T> inline void whileChance(int mChance, const T& mFn)
			{
				while(ssvu::getRnd(0, 100) < mChance)
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

	struct InstantEffect
	{
		enum class Type : int
		{
			Add = 0,
			Sub = 1,
			Mul = 2,
			Div = 3
		};

		enum class Stat : int
		{
			SHPS = 0,
			SATK = 1,
			SDEF = 2
		};

		Type type;
		Stat stat;
		float value;

		inline InstantEffect(Type mType, Stat mStat, float mValue) : type{mType}, stat{mStat}, value{mValue} { }
		inline void apply(GameSession& mGameSession, Creature& mX);

		inline std::string getStrType()
		{
			static auto array(ssvu::makeArray
			(
				"+",
				"-",
				"*",
				"/"
			));

			return array[static_cast<int>(type)];
		}

		inline std::string getStrStat()
		{
			static auto array(ssvu::makeArray
			(
				"HPS",
				"ATK",
				"DEF"
			));

			return array[static_cast<int>(stat)];
		}
	};

	struct Drop
	{
		GameSession& gameSession;
		sf::Sprite card;

		inline Drop(GameSession& mGameSession) : gameSession{mGameSession}
		{
			card.setTexture(*getAssets().itemCard);
			ssvs::setOrigin(card, ssvs::getLocalCenter);
		}

		inline virtual ~Drop() { }
		inline virtual void apply(Creature&) { }

		inline virtual void draw(ssvs::GameWindow& mGW, const Vec2f&, const Vec2f& mCenter)
		{
			card.setPosition(mCenter + Vec2f{0, -20.f});
			mGW.draw(card);
		}
	};


	inline auto createElemSprite(int mEI)
	{
		static auto array(ssvu::makeArray
		(
			getAssets().eFire,
			getAssets().eWater,
			getAssets().eEarth,
			getAssets().eLightning
		));

		return sf::Sprite{*(array[mEI])};
	}

	template<typename T> inline void appendElems(ssvs::GameWindow& mGW, const T& mX, ElementBitset mEB)
	{
		for(auto i(0u); i < Constants::elementCount; ++i)
		{
			if(!mEB[i]) continue;

			auto offset(7 * i);
			auto s(createElemSprite(i));

			s.setPosition(mX.getPosition() + Vec2f{12.f + offset, 0.f});

			mGW.draw(s);
		}
	}

	inline auto getStatDisplayStr(StatType mBase, StatType mBonus)
	{
		return ssvu::toStr(mBase + mBonus) + " (" + ssvu::toStr(mBase) + "+" + ssvu::toStr(mBonus) + ")";
	}

	struct StatRichText
	{
		ssvs::BitmapTextRich txt{*getAssets().fontObStroked};
		ssvs::BTRPart* pssExtra;
		ssvs::BTRPString* psTotal;
		ssvs::BTRPString* psBase;
		ssvs::BTRPString* psBonus;

		inline StatRichText()
		{
			txt.addTracking(-3);

			txt << sf::Color::White;
			psTotal = &txt.addStr("");

			pssExtra = &txt.addGroup();

			(*pssExtra) << sf::Color::White << " (";

			(*pssExtra) << sf::Color::Red;
			psBase = &pssExtra->addStr("");

			(*pssExtra) << sf::Color::White << "+";

			(*pssExtra) << sf::Color::Green;
			psBonus = &pssExtra->addStr("");

			(*pssExtra) << sf::Color::White << ")";
		}

		inline void set(StatType mX)
		{
			pssExtra->setEnabled(false);

			auto s(ssvu::toStr(mX));
			psTotal->setStr(s);
		}

		inline void set(StatType mBase, StatType mBonus)
		{
			pssExtra->setEnabled(true);

			auto sBase(ssvu::toStr(mBase));
			auto sBonus(ssvu::toStr(mBonus));
			auto sTotal(ssvu::toStr(mBase + mBonus));

			psTotal->setStr(sTotal);
			psBase->setStr(sBase);
			psBonus->setStr(sBonus);
		}
	};

	struct WeaponStatsDraw
	{
		Vec2f pos;
		sf::Sprite iconATK;
		sf::Sprite eST, eWK;
		StatRichText srtATK;

		inline WeaponStatsDraw()
		{
			iconATK.setTexture(*getAssets().iconATK);
			eST.setTexture(*getAssets().eST);
			eWK.setTexture(*getAssets().eWK);
		}

		inline void commonDraw(Weapon& mW, ssvs::GameWindow& mGW, const Vec2f& mPos, const Vec2f&)
		{
			iconATK.setPosition(mPos + pos);
			eST.setPosition(iconATK.getPosition() + Vec2f{0, 10 + 1});
			eWK.setPosition(eST.getPosition() + Vec2f{0, 6 + 1});
			srtATK.txt.setPosition(iconATK.getPosition() + Vec2f{12.f, 0});

			appendElems(mGW, eST, mW.strongAgainst);
			appendElems(mGW, eWK, mW.weakAgainst);

			mGW.draw(iconATK);
			mGW.draw(srtATK.txt);
			mGW.draw(eST);
			mGW.draw(eWK);
		}

		inline void draw(Weapon& mW, ssvs::GameWindow& mGW, const Vec2f& mPos, const Vec2f& mCenter)
		{
			srtATK.set(mW.atk);
			commonDraw(mW, mGW, mPos, mCenter);
		}

		inline void draw(Creature& mC, ssvs::GameWindow& mGW, const Vec2f& mPos, const Vec2f& mCenter)
		{
			srtATK.set(mC.weapon.atk, mC.bonusATK);
			commonDraw(mC.weapon, mGW, mPos, mCenter);
		}
	};

	struct ArmorStatsDraw
	{
		Vec2f pos;
		sf::Sprite iconDEF;
		sf::Sprite eTY;
		StatRichText srtDEF;

		inline ArmorStatsDraw()
		{
			iconDEF.setTexture(*getAssets().iconDEF);
			eTY.setTexture(*getAssets().eTY);
		}

		inline void commonDraw(Armor& mA, ssvs::GameWindow& mGW, const Vec2f& mPos, const Vec2f&)
		{
			iconDEF.setPosition(pos + mPos);
			eTY.setPosition(iconDEF.getPosition() + Vec2f{0, 10 + 1});
			srtDEF.txt.setPosition(iconDEF.getPosition() + Vec2f{12.f, 0});
			mGW.draw(iconDEF);
			mGW.draw(srtDEF.txt);
			mGW.draw(eTY);

			appendElems(mGW, eTY, mA.elementTypes);
		}

		inline void draw(Armor& mA, ssvs::GameWindow& mGW, const Vec2f& mPos, const Vec2f& mCenter)
		{
			srtDEF.set(mA.def);
			commonDraw(mA, mGW, mPos, mCenter);
		}

		inline void draw(Creature& mC, ssvs::GameWindow& mGW, const Vec2f& mPos, const Vec2f& mCenter)
		{
			srtDEF.set(mC.armor.def, mC.bonusDEF);
			commonDraw(mC.armor, mGW, mPos, mCenter);
		}
	};

	struct CreatureStatsDraw
	{
		sf::Sprite iconHPS;
		ssvs::BitmapText txtHPS;

		WeaponStatsDraw wsd;
		ArmorStatsDraw asd;

		inline CreatureStatsDraw() : txtHPS{mkTxtOBSmall()}
		{
			iconHPS.setTexture(*getAssets().iconHPS);
		}

		inline void draw(Creature& mC, ssvs::GameWindow& mGW, const Vec2f& mPos, const Vec2f& mCenter)
		{
			txtHPS.setString(ssvu::toStr(mC.hps));
			iconHPS.setPosition(mPos + Vec2f{0.f, 12.f * 0.f});
			txtHPS.setPosition(iconHPS.getPosition() + Vec2f{12.f, 0});

			wsd.pos = Vec2f{0, 12.f};
			wsd.draw(mC, mGW, mPos, mCenter);

			asd.pos = Vec2f{0, wsd.eWK.getPosition().y - mPos.y + 12.f};
			asd.draw(mC, mGW, mPos, mCenter);

			mGW.draw(iconHPS);
			mGW.draw(txtHPS);
		}
	};

	struct WeaponDrop : public Drop
	{
		Weapon weapon;
		WeaponStatsDraw wsd;
		sf::Sprite typeSprite;

		inline WeaponDrop(GameSession& mGameSession) : Drop{mGameSession}
		{
			card.setTexture(*getAssets().equipCard);
		}

		inline void apply(Creature& mX) override
		{
			getAssets().soundPlayer.play(*getAssets().equipWpn);
			mX.weapon = weapon;
		}

		inline void draw(ssvs::GameWindow& mGW, const Vec2f& mPos, const Vec2f& mCenter) override
		{
			Drop::draw(mGW, mPos, mCenter);

			typeSprite.setTexture(weapon.getTypeTexture());
			ssvs::setOrigin(typeSprite, ssvs::getLocalCenter);
			typeSprite.setPosition(card.getPosition());
			mGW.draw(typeSprite);

			wsd.pos = Vec2f{30 - 16, 30 + 6};
			wsd.draw(weapon, mGW, mPos, mCenter);
		}
	};

	struct ArmorDrop : public Drop
	{
		Armor armor;
		ArmorStatsDraw asd;
		sf::Sprite armorSprite;

		inline ArmorDrop(GameSession& mGameSession) : Drop{mGameSession}
		{
			card.setTexture(*getAssets().equipCard);
		}

		inline void apply(Creature& mX) override
		{
			getAssets().soundPlayer.play(*getAssets().equipArmor);
			mX.armor = armor;
		}

		inline void draw(ssvs::GameWindow& mGW, const Vec2f& mPos, const Vec2f& mCenter) override
		{
			Drop::draw(mGW, mPos, mCenter);

			armorSprite.setTexture(*getAssets().armDrop);
			ssvs::setOrigin(armorSprite, ssvs::getLocalCenter);
			armorSprite.setPosition(card.getPosition());
			mGW.draw(armorSprite);

			asd.pos = Vec2f{30 - 16, 30 + 6};
			asd.draw(armor, mGW, mPos, mCenter);
		}
	};

	struct DropIE : public Drop
	{
		std::vector<InstantEffect> ies;
		std::vector<ssvs::BitmapText> bts;

		inline DropIE(GameSession& mGameSession) : Drop{mGameSession} { }

		inline void addIE(InstantEffect mIE)
		{
			ies.emplace_back(mIE);

			ssvs::BitmapText txt{mkTxtOBSmall()};
			txt.setString(mIE.getStrType() + ssvu::toStr(static_cast<int>(mIE.value)) + " " + mIE.getStrStat());
			ssvs::setOrigin(txt, ssvs::getLocalCenter);

			bts.emplace_back(txt);
		}

		inline void apply(Creature& mX) override
		{
			getAssets().soundPlayer.play(*getAssets().powerup, ssvs::SoundPlayer::Mode::Overlap, 1.8f);
			for(auto& x : ies) x.apply(gameSession, mX);
		}

		inline void draw(ssvs::GameWindow& mGW, const Vec2f& mPos, const Vec2f& mCenter) override
		{
			Drop::draw(mGW, mPos, mCenter);

			int i{0};
			for(auto& t : bts)
			{
				t.setPosition(card.getPosition() + Vec2f{0, -15.f + (10 * i)});
				mGW.draw(t);

				++i;
			}
		}
	};



	struct ItemDrops
	{
		ssvu::UPtr<Drop> drops[Constants::maxDrops];

		inline ItemDrops()
		{
			for(auto i(0u); i < Constants::maxDrops; ++i)
				drops[i] = nullptr;
		}

		inline bool has(int mIdx)
		{
			return drops[mIdx] != nullptr;
		}

		inline void give(int mIdx, Creature& mX)
		{
			drops[mIdx]->apply(mX);
			drops[mIdx].release();
		}
	};

	class GameSession;

	struct Choice
	{
		GameSession& gameSession;
		SizeT idx;

		inline Choice(GameSession& mGameState, SizeT mIdx) : gameSession{mGameState}, idx{mIdx} { }
		inline virtual ~Choice() { }

		inline virtual void execute() { }
		inline virtual void draw(ssvs::GameWindow&, const Vec2f&, const Vec2f&) { }

		inline virtual std::string getChoiceStr() { return ""; }
	};

	struct ChoiceAdvance : public Choice
	{
		sf::Sprite advanceSprite;

		inline ChoiceAdvance(GameSession& mGameState, SizeT mIdx) : Choice{mGameState, mIdx}
		{
			advanceSprite.setTexture(*getAssets().advance);
			ssvs::setOrigin(advanceSprite, ssvs::getLocalCenter);
		}

		inline void execute() override;
		inline void draw(ssvs::GameWindow&, const Vec2f&, const Vec2f&) override;

		inline std::string getChoiceStr() override { return "Forward"; }
	};


	struct ChoiceCreature : public Choice
	{
		Creature creature;
		CreatureStatsDraw csd;
		sf::Sprite enemySprite;
		float hoverRads;

		inline ChoiceCreature(GameSession& mGameState, SizeT mIdx)
			: Choice{mGameState, mIdx}
		{
			enemySprite.setTexture(*getAssets().enemy);
			ssvs::setOrigin(enemySprite, ssvs::getLocalCenter);
			hoverRads = ssvu::getRndR(0.f, ssvu::tau);
		}

		inline void execute() override;
		inline void draw(ssvs::GameWindow&, const Vec2f&, const Vec2f&) override;

		inline std::string getChoiceStr() override { return "Fight"; }
	};

	struct ChoiceItemDrop : public Choice
	{
		sf::Sprite drops;
		ItemDrops itemDrops;

		ChoiceItemDrop(GameSession& mGS, SizeT mIdx);

		inline void execute() override;
		inline void draw(ssvs::GameWindow&, const Vec2f&, const Vec2f&) override;

		inline std::string getChoiceStr() override { return "Collect"; }
	};

	struct ChoiceSingleDrop : public Choice
	{
		ssvu::UPtr<Drop> drop{nullptr};

		ChoiceSingleDrop(GameSession& mGS, SizeT mIdx);

		inline void execute() override;
		inline void draw(ssvs::GameWindow&, const Vec2f&, const Vec2f&) override;

		inline std::string getChoiceStr() override { return "Pickup"; }
	};

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

	inline void ChoiceAdvance::execute()
	{
		gameSession.advance();
	}
	inline void ChoiceAdvance::draw(ssvs::GameWindow& mGW, const Vec2f&, const Vec2f& mCenter)
	{
		advanceSprite.setPosition(mCenter);
		mGW.draw(advanceSprite);
	}

	inline ChoiceItemDrop::ChoiceItemDrop(GameSession& mGS, SizeT mIdx) : Choice{mGS, mIdx}
	{
		drops.setTexture(*getAssets().drops);
		itemDrops = mGS.generateDrops(mGS.roomNumber);
	}
	inline void ChoiceItemDrop::execute()
	{
		getAssets().soundPlayer.play(*getAssets().grab);
		gameSession.startDrops(&itemDrops);
		gameSession.resetChoiceAt(idx, ssvu::makeUPtr<ChoiceAdvance>(gameSession, idx));
	}
	inline void ChoiceItemDrop::draw(ssvs::GameWindow& mGW, const Vec2f& mPos, const Vec2f&)
	{
		drops.setPosition(mPos);
		mGW.draw(drops);
	}

	inline ChoiceSingleDrop::ChoiceSingleDrop(GameSession& mGS, SizeT mIdx) : Choice{mGS, mIdx}
	{

	}
	inline void ChoiceSingleDrop::execute()
	{
		if(drop == nullptr) return;

		drop->apply(gameSession.player);
		gameSession.resetChoiceAt(idx, ssvu::makeUPtr<ChoiceAdvance>(gameSession, idx));
	}
	inline void ChoiceSingleDrop::draw(ssvs::GameWindow& mGW, const Vec2f& mPos, const Vec2f& mCenter)
	{
		if(drop == nullptr) return;

		drop->draw(mGW, mPos, mCenter);
	}


	inline void ChoiceCreature::execute()
	{
		gameSession.player.weapon.playAttackSounds();

		if(gameSession.player.canDamage(creature))
		{
			gameSession.player.fight(creature);

			gameSession.sustain();

			getAssets().soundPlayer.play(*getAssets().drop);
			gameSession.resetChoiceAt(idx, ssvu::makeUPtr<ChoiceItemDrop>(gameSession, idx));

			gameSession.shake = 10;
		}
		else
		{
			eventLo() << gameSession.player.name << " cannot fight " << creature.name << "!\n";
		}
	}
	inline void ChoiceCreature::draw(ssvs::GameWindow& mGW, const Vec2f& mPos, const Vec2f& mCenter)
	{
		Vec2f offset{4.f, 4.f};
		hoverRads = ssvu::wrapRad(hoverRads + 0.05f);
		enemySprite.setPosition(mCenter + Vec2f(0, std::sin(hoverRads) * 4.f));
		mGW.draw(enemySprite);
		csd.draw(creature, mGW, offset + mPos, mCenter);
	}

	inline void InstantEffect::apply(GameSession& mGameSession, Creature& mX)
	{
		StatType* statPtr{nullptr};

		switch(stat)
		{
			case Stat::SHPS: statPtr = &mX.hps; break;
			case Stat::SATK: statPtr = &mX.bonusATK; break;
			case Stat::SDEF: statPtr = &mX.bonusDEF; break;
		}

		float x(static_cast<float>(*statPtr));

		switch(type)
		{
			case Type::Add: *statPtr += value; break;
			case Type::Sub: *statPtr -= value; break;
			case Type::Mul: *statPtr = static_cast<int>(x * value); break;
			case Type::Div: *statPtr = static_cast<int>(x / value); break;
		}

		eventLo() << "Got " << getStrType() << ssvu::toStr(static_cast<int>(value)) << " " << getStrStat() << "!\n";

		mX.checkBurns(mGameSession);
	}

	inline void Creature::checkBurns(GameSession& mGameSession)
	{
		int burn{0};

		if(bonusATK < 0)
		{
			burn -= bonusATK;
			bonusATK = 0;
		}

		if(bonusDEF < 0)
		{
			burn -= bonusDEF;
			bonusDEF = 0;
		}

		if(burn == 0) return;

		auto x(burn * (4 * mGameSession.roomNumber * mGameSession.difficulty));

		hps -= x;
		eventLo() << name << " suffers " << x << " stat burn dmg!\n";
	}

	struct SlotChoice
	{
		sf::RectangleShape shape;
		sf::Sprite sprite;
		ssvs::BitmapText txtNum;
		ssvs::BitmapText txtStr;
		int choice;

		static constexpr float step{300.f / 4.f};

		inline SlotChoice(int mChoice) : txtNum{*getAssets().fontObBig, ssvu::toStr(mChoice + 1)},
			txtStr{mkTxtOBSmall()}, choice{mChoice}
		{
			shape.setSize(Vec2f{step, 130.f});
			shape.setFillColor(sf::Color::Red);
			shape.setPosition(Vec2f{10 + step * mChoice, 40});

			sprite.setTexture(*getAssets().slotChoice);
			sprite.setPosition(Vec2f{10 + step * mChoice, 40});

			ssvs::setOrigin(txtNum, ssvs::getLocalCenter);
			txtNum.setPosition(Vec2f{10 + step * mChoice + (step / 2.f), 40 + 105});
		}

		inline void update()
		{
			ssvs::setOrigin(txtStr, ssvs::getLocalCenter);
			txtStr.setPosition(Vec2f{10 + step * choice + (step / 2.f), 40 + 120});
		}

		inline Vec2f getCenter()
		{
			return Vec2f{10 + (step * choice) + (step / 2.f), 40 + 130.f / 2.f};
		}

		inline void drawInCenter(ssvs::GameWindow& mGW, const sf::Texture& mX)
		{
			sf::Sprite s;
			s.setTexture(mX);
			ssvs::setOrigin(s, ssvs::getLocalCenter);
			s.setPosition(getCenter());
			mGW.draw(s);
		}
	};

	class GameApp : public Boilerplate::App
	{
		private:
			GameSession gs;
			ssvs::BitmapText txtTimer{mkTxtOBBig()}, txtRoom{mkTxtOBBig()}, txtDeath{mkTxtOBBig()},
							txtLog{mkTxtOBSmall()}, txtRestart{mkTxtOBSmall()}, txtMode{mkTxtOBSmall()};

			ssvs::BitmapTextRich txtCredits{*getAssets().fontObStroked};
			std::vector<SlotChoice> slotChoices;
			sf::Sprite dropsModalSprite;
			CreatureStatsDraw csdPlayer;
			Vec2f oldPos;

			inline void initInput()
			{
				auto& gState(gameState);

				// TODO: better input management, choose handling type
				gState.addInput({{IK::Escape}}, [this](FT){ if(gs.state != GameSession::State::Menu) gs.gotoMenu(); }, IT::Once);

				gState.addInput({{IK::A}}, [this](FT){ gameCamera.pan(-4, 0); });
				gState.addInput({{IK::D}}, [this](FT){ gameCamera.pan(4, 0); });
				gState.addInput({{IK::W}}, [this](FT){ gameCamera.pan(0, -4); });
				gState.addInput({{IK::S}}, [this](FT){ gameCamera.pan(0, 4); });
				gState.addInput({{IK::Q}}, [this](FT){ gameCamera.zoomOut(1.1f); });
				gState.addInput({{IK::E}}, [this](FT){ gameCamera.zoomIn(1.1f); });

				gState.addInput({{IK::Num1}}, [this](FT){ executeChoice(0); }, IT::Once);
				gState.addInput({{IK::Num2}}, [this](FT){ executeChoice(1); }, IT::Once);
				gState.addInput({{IK::Num3}}, [this](FT){ executeChoice(2); }, IT::Once);
				gState.addInput({{IK::Num4}}, [this](FT){ executeChoice(3); }, IT::Once);
			}

			inline void executeChoice(int mI)
			{
				if(gs.state == GameSession::State::Menu)
				{
					if(mI == 0)
					{
						gs.mode = GameSession::Mode::Beginner;
						gs.restart();
					}

					if(mI == 1)
					{
						gs.mode = GameSession::Mode::Official;
						gs.restart();
					}

					if(mI == 2)
					{
						gs.mode = GameSession::Mode::Hardcore;
						gs.restart();
					}

					if(mI == 3)
					{
						stop();
					}

					return;
				}

				if(gs.state == GameSession::State::Dead)
				{
					if(mI == 0) gs.gotoMenu();
					else if(mI == 1) gs.restart();

					return;
				}

				if(gs.currentDrops == nullptr)
				{
					if(gs.choices[mI] == nullptr) return;

					gs.choices[mI]->execute();

					if(gs.currentDrops == nullptr)
						gs.refreshChoices();
				}
				else
				{
					if(mI == 0)
					{
						gs.endDrops();
						gs.refreshChoices();
					}
					else
					{
						gs.tryPickupDrop(mI - 1);
					}
				}
			}

			inline void update(FT mFT)
			{
				gameCamera.update<float>(mFT);

				if(gs.deathTextTime > 0) gs.deathTextTime -= mFT;

				if(gs.state == GameSession::State::Playing)
				{
					if(gs.timerEnabled) gs.timer -= mFT;

					if(!gs.player.isDead())
					{
						auto secs(ssvu::getFTToSeconds(gs.timer));
						if(secs < 3) ssvu::clampMin(gs.shake, 4 - secs);
					}

					if(gs.timer <= 0 || gs.player.isDead())
					{
						gs.die();
					}
					else
					{
						auto intt(ssvu::getFTToSeconds(static_cast<int>(gs.timer)));
						auto gts(intt >= 10 ? ssvu::toStr(intt) : "0" + ssvu::toStr(intt));

						auto third(gameWindow.getWidth() / 5.f);

						txtTimer.setString(gs.timerEnabled ? "00:" + gts : "XX:XX");

						ssvs::setOrigin(txtTimer, ssvs::getLocalCenter);
						txtTimer.setPosition(third * 1.f, 20);

						txtRoom.setString("Room:" + ssvu::toStr(gs.roomNumber));
						ssvs::setOrigin(txtRoom, ssvs::getLocalCenter);
						txtRoom.setPosition(third * 4.f, 20);

						// TODO: wtf
						auto els(getEventLogStream().str());
						if(!els.empty())
						{
							std::string elsLog;

							int foundNewLines{0};

							for(auto itr(els.rbegin()); itr < els.rend(); ++itr)
							{
								if(*itr == '\n') ++foundNewLines;
								if(foundNewLines == 6) break;
								elsLog += *itr;
							}

							std::string final{elsLog.rbegin(), elsLog.rend()};
							txtLog.setString(final);
						}
					}
				}
				else if(gs.state == GameSession::State::Menu)
				{

				}

				if(gs.shake > 0)
				{
					gs.shake -= mFT;
					auto shake(std::abs(gs.shake));
					gameCamera.setCenter(oldPos + Vec2f{ssvu::getRndR(-shake, shake + 0.1f), ssvu::getRndR(-shake, shake + 0.1f)});
				}
				else
				{
					gameCamera.setCenter(oldPos);
				}
			}

			inline const auto& getModeStr()
			{
				static auto array(ssvu::makeArray
				(
					"Beginner mode",
					"Official mode",
					"Hardcore mode"
				));

				return array[static_cast<int>(gs.mode)];
			}

			inline void drawPlaying()
			{
				txtMode.setString(getModeStr());
				ssvs::setOrigin(txtMode, ssvs::getLocalCenterS);
				txtMode.setPosition(320 / 2.f, 40 - 2);

				render(txtTimer);
				render(txtRoom);
				render(txtMode);

				if(gs.currentDrops != nullptr)
				{
					render(dropsModalSprite);

					for(auto i(0u); i < slotChoices.size(); ++i)
					{
						auto& sc(slotChoices[i]);

						if(i == 0)
						{
							sc.drawInCenter(gameWindow, *getAssets().back);
							sc.txtStr.setString("Back");
						}
						else if(gs.currentDrops->has(i - 1))
						{
							gs.currentDrops->drops[i -1]->draw(gameWindow, sc.shape.getPosition(), sc.getCenter());
							sc.txtStr.setString("Pickup");
						}

						sc.update();

						if(i == 0 || gs.currentDrops->has(i - 1))
						{
							render(sc.txtNum);
							render(sc.txtStr);
						}
					}
				}
				else
				{
					for(auto i(0u); i < slotChoices.size(); ++i)
					{
						auto& sc(slotChoices[i]);
						const auto& gc(gs.choices[i]);

						sc.txtStr.setString(gc == nullptr ? "Blocked" : gc->getChoiceStr());
						sc.update();

						render(sc.shape);
						render(sc.sprite);

						if(gc != nullptr)
						{
							gc->draw(gameWindow, sc.shape.getPosition(), sc.getCenter());
						}
						else
						{
							sc.drawInCenter(gameWindow, *getAssets().blocked);
						}

						render(sc.txtNum);
						render(sc.txtStr);
					}
				}

				render(txtLog);

				csdPlayer.draw(gs.player, gameWindow, Vec2f{10, 175}, Vec2f{0.f, 0.f});
			}

			inline void draw()
			{
				gameCamera.apply();

				if(gs.state == GameSession::State::Playing || gs.deathTextTime > 0)
					drawPlaying();

				gameCamera.unapply();

				ssvs::setOrigin(txtDeath, ssvs::getLocalCenter);
				ssvs::setOrigin(txtRestart, ssvs::getLocalCenter);
				ssvs::setOrigin(txtCredits, ssvs::getLocalSW);

				txtCredits.setPosition(5, 240 - 5);

				if(gs.state == GameSession::State::Dead)
				{
					txtDeath.setString("You have perished.");
					txtRestart.setString("Press 1 for menu.\n"
										 "Press 2 to restart.\n\n"
										 "You reached room " + ssvu::toStr(gs.roomNumber) + ".\n"
										 "(" + getModeStr() + ")");

					txtDeath.setPosition(320 / 2.f, 80);
					txtRestart.setPosition(320 / 2.f, 120);

					render(txtDeath);
					render(txtRestart);

					txtDeath.setColor(sf::Color(255, 255, 255, 255 - static_cast<unsigned char>(gs.deathTextTime)));
					txtRestart.setColor(sf::Color(255, 255, 255, 255 - static_cast<unsigned char>(gs.deathTextTime)));
				}

				if(gs.state == GameSession::State::Menu)
				{
					txtDeath.setString("DELVER'S CHOICE");
					txtDeath.setColor(sf::Color(255, 255, 255, 255));

					txtDeath.setPosition(320 / 2.f, 30);
					txtRestart.setPosition(320 / 2.f, 70);

					txtRestart.setString("1. Beginner mode\n"
										 "2. Official mode\n"
										 "3. Hardcore mode\n"
										 "4. Exit game");

					txtRestart.setColor(sf::Color(255, 255, 255, 255));

					render(txtDeath);
					render(txtRestart);
					render(txtCredits);
				}
			}

		public:
			inline GameApp(ssvs::GameWindow& mGameWindow) : Boilerplate::App{mGameWindow}
			{
				using sfc = sf::Color;

				txtCredits.addTracking(-3);

				txtCredits
					<< sfc::White  << "Global Game Jam 2015\n"
					<< sfc::White << "Developer: " << sfc::Red << "Vittorio Romeo\n"
					<< sfc::White << "2D Artist: " << sfc::Red << "Vittorio Romeo\n"
					<< sfc::White << "Audio: " << sfc::Red << "Nicola Bombaci\n"
					<< sfc::White << "Designer: " << sfc::Red << "Sergio Zavettieri\n"
					<< sfc::White << "Additional help: " << sfc::Red << "Davide Iuffrida\n"
					<< sfc::Blue << "http://vittorioromeo.info\nhttp://nicolabombaci.com";


				for(int i{0}; i < 4; ++i) slotChoices.emplace_back(i);

				gameState.onUpdate += [this](FT mFT){ update(mFT); };
				gameState.onDraw += [this]{ draw(); };

				dropsModalSprite.setTexture(*getAssets().dropsModal);
				dropsModalSprite.setPosition(10, 40);

				txtLog.setPosition(Vec2f{75, 180});

				initInput();

				oldPos = gameCamera.getCenter();

				gs.gotoMenu();
			}
	};
}

int main()
{
	SSVUT_RUN();

	Boilerplate::AppRunner<ggj::GameApp>{"Delver's choice - GGJ2015 - RC6", 320, 240};
	return 0;
}
