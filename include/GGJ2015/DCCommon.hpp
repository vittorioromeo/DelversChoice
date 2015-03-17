#ifndef GGJ2015_DCCOMMON
#define GGJ2015_DCCOMMON

#include "../GGJ2015/Common.hpp"
#include "../GGJ2015/Boilerplate.hpp"


namespace Exp
{
	using namespace ssvs;

	namespace Impl
	{
		class BTRRoot;
		class BTRChunk;

		class BTREffect
		{
			friend class BTRRoot;

			public:
				inline virtual void update(FT) noexcept { }
				inline virtual void apply(BTRChunk&) noexcept { }
		};

		template<typename T> struct EffectHelper;

		class BTRChunk
		{
			template<typename> friend struct Impl::EffectHelper;
			friend class BTRRoot;

			private:
				BTRRoot& root;
				BTRChunk* parent{nullptr};
				std::string str;
				std::vector<BTREffect*> childrenEffects;
				std::vector<BTRChunk*> children;
				SizeT idxHierarchyBegin, idxHierarchyEnd;

				float trackingModifier{0.f}, leadingModifier{0.f}, hChunkSpacingModifier{0.f};

				template<typename... TArgs> BTRChunk& mkChild(TArgs&&... mArgs);
				template<typename T, typename... TArgs> T& mkEffect(TArgs&&... mArgs);

				void refreshGeometry() noexcept;
				void refreshEffects() noexcept;

				inline void update(FT mFT) noexcept
				{
					for(auto& e : childrenEffects) e->update(mFT);
					for(auto& c : children) c->update(mFT);
				}

			public:
				inline BTRChunk(BTRRoot& mRoot) noexcept : root{mRoot} { }

				inline float getTracking() const noexcept		{ return parent == nullptr ? 0.f : parent->getTracking() + parent->trackingModifier; }
				inline float getLeading() const noexcept		{ return parent == nullptr ? 0.f : parent->getLeading() + parent->leadingModifier; }
				inline float getHChunkSpacing() const noexcept	{ return parent == nullptr ? 0.f : parent->getHChunkSpacing() + parent->hChunkSpacingModifier; }

				template<typename TF> void forVertices(TF mFn) noexcept;

				template<typename T> void setStr(T&& mX);
				inline const auto& getStr() const noexcept { return str; }

				// Make children and go one level deeper
				auto& in();
				auto& in(BTRChunk*&);
				template<typename T> auto& in(T&& mStr);
				template<typename T> auto& in(BTRChunk*&, T&& mStr);

				// Make children and stay on the same level
				auto& mk(BTRChunk*&);
				template<typename T> auto& mk(T&& mStr);
				template<typename T> auto& mk(BTRChunk*&, T&& mStr);

				// Add effects
				auto& eff(const sf::Color&);
				template<typename T, typename... TArgs> auto& eff(TArgs&&...);
				template<typename T, typename... TArgs> auto& eff(T*&, TArgs&&...);

				// Go back one level
				inline auto& out() const noexcept { SSVU_ASSERT(parent != nullptr); return *parent; }
		};



		using BTRChunkRecycler = ssvu::MonoRecycler<BTRChunk>;
		using BTRChunkPtr = typename BTRChunkRecycler::PtrType;

		using BTREffectRecycler = ssvu::PolyRecycler<BTREffect>;
		using BTREffectPtr = typename BTREffectRecycler::PtrType;

		class BTREWave : public BTREffect
		{
			private:
				float angle, amplitude, repeat, speedMult;

			public:
				inline BTREWave(float mAmplitude = 2.f, float mSpeedMult = 0.1f, float mRepeat = 4.f, float mAngleStart = 0.f)
					: angle{mAngleStart}, amplitude{mAmplitude}, repeat{mRepeat}, speedMult{mSpeedMult} { }

				inline void update(FT mFT) noexcept override { angle = ssvu::getWrapRad(angle + mFT * speedMult); }
				inline void apply(BTRChunk& mX) noexcept override
				{
					mX.forVertices([this](auto mIdx, auto, auto& mV, auto& mVO)
					{
						mV.position.y = mVO.position.y + std::sin(angle + (mIdx / repeat)) * amplitude;
					});
				}
		};

		class BTREColorFG : public BTREffect
		{
			public:
				enum class Anim{None, Pulse};

			private:
				sf::Color colorFG, colorFGComputed;
				float pulse{0.f}, pulseSpeed, pulseMax;
				Anim anim{Anim::None};

			public:
				inline BTREColorFG(const sf::Color& mColorFG) noexcept : colorFG{mColorFG}, colorFGComputed{colorFG} { }

				inline void update(FT mFT) noexcept override
				{
					if(anim == Anim::Pulse)
					{
						pulse = ssvu::getWrapRad(pulse + (mFT * pulseSpeed));
						colorFGComputed = colorFG;
						colorFGComputed.a = ssvu::toInt(255.f - std::abs((std::sin(pulse) * pulseMax)));
					}
				}
				inline void apply(BTRChunk& mX) noexcept override
				{
					mX.forVertices([this](auto, auto, auto& mV, auto&){ mV.color = colorFGComputed; });
				}

				inline void setAnimNone() noexcept { anim = Anim::None; }
				inline void setAnimPulse(float mSpeed, float mMax, float mStart = 0.f) noexcept
				{
					anim = Anim::Pulse;
					pulse = mStart;
					pulseSpeed = mSpeed;
					pulseMax = mMax;
				}
		};

		struct BTRDrawState
		{
			struct RowData
			{
				float width;
				SizeT cells;

				inline RowData(float mWidth, SizeT mCells) noexcept : width{mWidth}, cells{mCells} { }
			};

			std::vector<RowData> rDatas;
			float xMin, xMax, yMin, yMax, nextHChunkSpacing;
			SizeT width, height, iX;
			int nl, htab, vtab;

			inline void reset(const BitmapFont& mBF) noexcept
			{
				rDatas.clear();

				xMin = xMax = yMin = yMax = nextHChunkSpacing = 0.f;

				width = mBF.getCellWidth();
				height = mBF.getCellHeight();
				iX = 0;

				nl = htab = vtab = 0;
			}
		};



		class BTRRoot : public sf::Transformable, public sf::Drawable
		{
			friend class BTRChunk;
			template<typename> friend struct Impl::EffectHelper;

			private:
				const BitmapFont* bitmapFont{nullptr};
				const sf::Texture* texture{nullptr};
				mutable VertexVector<sf::PrimitiveType::Quads> vertices, verticesOriginal;
				mutable sf::FloatRect bounds, globalBounds;
				mutable bool mustRefreshGeometry{true};

				// TODO: abstract
				BTRChunkRecycler chunkRecycler;
				std::vector<BTRChunkPtr> chunkManager;

				// TODO: abstract
				BTREffectRecycler effectRecycler;
				std::vector<BTREffectPtr> effectManager;

				BTRChunk* baseChunk{&mkChunk()};
				float alignMult{0.f};

				mutable BTRDrawState bdd;

				inline void refreshIfNeeded() const
				{
					refreshGeometryIfNeeded();
					baseChunk->refreshEffects();
				}

				inline void refreshGeometryIfNeeded() const
				{
					if(!mustRefreshGeometry) return;
					mustRefreshGeometry = false;

					refreshGeometryStart();
					baseChunk->refreshGeometry();
					verticesOriginal = vertices;
					refreshGeometryFinish();
				}

				template<typename... TArgs> inline BTRChunk& mkChunk(TArgs&&... mArgs)			{ return chunkRecycler.getCreateEmplace(chunkManager, *this, FWD(mArgs)...); }
				template<typename T, typename... TArgs> inline T& mkEffect(TArgs&&... mArgs)	{ return effectRecycler.getCreateEmplace<T>(effectManager, FWD(mArgs)...); }

				inline void pushRowData() const
				{
					bdd.rDatas.emplace_back(vertices.back().position.x, bdd.iX);
				}

				inline void refreshGeometryStart() const noexcept
				{
					SSVU_ASSERT(bitmapFont != nullptr);

					vertices.clear();
					verticesOriginal.clear();
					bdd.reset(*bitmapFont);
				}

				inline void refreshGeometryFinish() const
				{
					// Push last row data
					pushRowData();

					// Recalculate bounds
					auto width(bdd.xMax - bdd.xMin);
					bounds = {bdd.xMin, bdd.yMin, width, bdd.yMax - bdd.yMin};
					globalBounds = getTransform().transformRect(bounds);

					// Apply horizontal alignment
					SizeT lastVIdx{0};
					for(const auto& rd : bdd.rDatas)
					{
						auto targetVIdx(lastVIdx + rd.cells * 4);
						auto offset(width - rd.width);

						for(; lastVIdx < targetVIdx; ++lastVIdx) vertices[lastVIdx].position.x += offset * alignMult;
					}
				}

				inline void mkVertices(BTRChunk& mChunk) const
				{
					const auto& str(mChunk.str);
					bdd.nextHChunkSpacing = mChunk.getHChunkSpacing();
					mChunk.idxHierarchyBegin = vertices.size();

					for(const auto& c : str)
					{
						switch(c)
						{
							case L'\n':  ++bdd.nl;		continue;
							case L'\t':  ++bdd.htab;	continue;
							case L'\v':  ++bdd.vtab;	continue;
						}

						const auto& tracking(mChunk.getTracking());
						const auto& leading(mChunk.getLeading());
						const auto& rect(bitmapFont->getGlyphRect(c));

						auto newPos(vertices.empty() ? Vec2f(0.f, bdd.height) : vertices.back().position);

						newPos.x += bdd.nextHChunkSpacing;
						bdd.nextHChunkSpacing = 0.f;

						if(bdd.nl > 0)
						{
							pushRowData();

							bdd.iX = 0;
							newPos.x = 0;

							for(; bdd.nl > 0; --bdd.nl) newPos.y += bdd.height + leading;
						}

						newPos.x += tracking;
						for(; bdd.htab > 0; --bdd.htab) newPos.x += 4 * (bdd.width + tracking);
						for(; bdd.vtab > 0; --bdd.vtab) newPos.y += 4 * (bdd.height + leading);

						auto gLeft(newPos.x);
						auto gBottom(newPos.y);
						auto gRight(gLeft + bdd.width);
						auto gTop(gBottom - bdd.height);

						ssvu::clampMax(bdd.xMin, gLeft);
						ssvu::clampMin(bdd.xMax, gRight);
						ssvu::clampMax(bdd.yMin, gTop);
						ssvu::clampMin(bdd.yMax, gBottom);

						vertices.emplace_back(Vec2f(gRight, gTop),		Vec2f(rect.left + rect.width,	rect.top));
						vertices.emplace_back(Vec2f(gLeft, gTop),		Vec2f(rect.left,				rect.top));
						vertices.emplace_back(Vec2f(gLeft, gBottom),	Vec2f(rect.left,				rect.top + rect.height));
						vertices.emplace_back(Vec2f(gRight, gBottom),	Vec2f(rect.left + rect.width,	rect.top + rect.height));

						++bdd.iX;
					}

					mChunk.idxHierarchyEnd = vertices.size();
				}

			public:
				inline BTRRoot() noexcept { }
				inline BTRRoot(const BitmapFont& mBF) noexcept : bitmapFont{&mBF}, texture{&bitmapFont->getTexture()} { }

				inline void clear()
				{
					mustRefreshGeometry = true;
					chunkManager.clear();
					effectManager.clear();
					baseChunk = &mkChunk();
				}
				inline void update(FT mFT) noexcept
				{
					baseChunk->update(mFT);
				}

				template<typename... TArgs> inline decltype(auto) in(TArgs&&... mArgs) { return baseChunk->in(FWD(mArgs)...); }
				template<typename T, typename... TArgs> inline decltype(auto) eff(TArgs&&... mArgs) { return baseChunk->eff<T>(FWD(mArgs)...); }

				inline void setAlign(TextAlign mX) noexcept
				{
					auto newAlignMult(ssvu::toFloat(ssvu::castEnum(mX)) * 0.5f);

					if(alignMult == newAlignMult) return;

					alignMult = newAlignMult;
					mustRefreshGeometry = true;
				}

				inline void draw(sf::RenderTarget& mRenderTarget, sf::RenderStates mRenderStates) const override
				{
					SSVU_ASSERT(bitmapFont != nullptr && texture != nullptr);

					refreshIfNeeded();

					mRenderStates.texture = texture;
					mRenderStates.transform *= getTransform();
					mRenderTarget.draw(vertices, mRenderStates);
				}

				inline const auto& getBitmapFont() const noexcept	{ return bitmapFont; }
				inline const auto& getLocalBounds() const			{ refreshGeometryIfNeeded(); return bounds; }
				inline auto getGlobalBounds() const					{ refreshGeometryIfNeeded(); return globalBounds; }
		};
	}

	namespace BS
	{
		struct Tracking { };
		struct Leading { };
		struct HChunkSpacing { };
		struct Pulse { };
		struct PulseDef { };
		using ColorFG = Impl::BTREColorFG;
		using Wave = Impl::BTREWave;
		using Chunk = Impl::BTRChunk;
	}

	using BitmapTextRich = Impl::BTRRoot;

	namespace Impl
	{
		template<typename TF> inline void BTRChunk::forVertices(TF mFn) noexcept
		{
			auto count(idxHierarchyEnd - idxHierarchyBegin);

			for(auto i(0u); i < count; ++i)
				mFn(i, count, root.vertices[idxHierarchyBegin + i], root.verticesOriginal[idxHierarchyBegin + i]);
		}

		inline void BTRChunk::refreshGeometry() noexcept
		{
			root.mkVertices(*this);
			for(auto& c : children)
			{
				c->refreshGeometry();

				ssvu::clampMax(idxHierarchyBegin, c->idxHierarchyBegin);
				ssvu::clampMin(idxHierarchyEnd, c->idxHierarchyEnd);
			}
		}
		inline void BTRChunk::refreshEffects() noexcept
		{
			if(parent != nullptr)
				for(auto& e : parent->childrenEffects) e->apply(*this);

			for(auto& c : children) c->refreshEffects();
		}

		template<typename... TArgs> inline BTRChunk& BTRChunk::mkChild(TArgs&&... mArgs)
		{
			auto& result(root.mkChunk(FWD(mArgs)...));
			result.parent = this;
			children.emplace_back(&result);
			return result;
		}
		template<typename T, typename... TArgs> inline T& BTRChunk::mkEffect(TArgs&&... mArgs)
		{
			auto& result(root.mkEffect<T>(FWD(mArgs)...));
			childrenEffects.emplace_back(&result);
			return result;
		}

		template<typename T> inline auto& BTRChunk::in(T&& mStr)
		{
			auto& result(mkChild());
			result.str = FWD(mStr);

			root.mustRefreshGeometry = true;
			return result;
		}
		inline auto& BTRChunk::in(BTRChunk*& mPtr)
		{
			auto& result(in(""));
			mPtr = &result;
			return result;
		}
		template<typename T> inline auto& BTRChunk::in(BTRChunk*& mPtr, T&& mStr)
		{
			auto& result(in(FWD(mStr)));
			mPtr = &result;
			return result;
		}
		inline auto& BTRChunk::in() { return in(""); }

		template<typename T> inline auto& BTRChunk::mk(T&& mStr)					{ in(FWD(mStr)); return *this; }
		template<typename T> inline auto& BTRChunk::mk(BTRChunk*& mPtr, T&& mStr)	{ in(mPtr, FWD(mStr)); return *this; }
		inline auto& BTRChunk::mk(BTRChunk*& mPtr)									{ in(mPtr, ""); return *this; }

		inline auto& BTRChunk::eff(const sf::Color& mX)													{ mkEffect<BTREColorFG>(mX); return *this; }
		template<typename T, typename... TArgs> inline auto& BTRChunk::eff(TArgs&&... mArgs)			{ EffectHelper<T>::mk(*this, FWD(mArgs)...); return *this; }
		template<typename T, typename... TArgs> inline auto& BTRChunk::eff(T*& mPtr, TArgs&&... mArgs)	{ mPtr = &EffectHelper<T>::mk(*this, FWD(mArgs)...); return *this; }

		template<typename T> inline void BTRChunk::setStr(T&& mX) { str = FWD(mX); root.mustRefreshGeometry = true; }

		template<typename T> struct EffectHelper
		{
			template<typename... TArgs> inline static auto& mk(BTRChunk& mC, TArgs&&... mArgs)
			{
				return mC.mkEffect<T>(FWD(mArgs)...);
			}
		};
		template<> struct EffectHelper<BS::Tracking>
		{
			inline static void mk(BTRChunk& mC, float mX)
			{
				mC.trackingModifier = mX;
				mC.root.mustRefreshGeometry = true;
			}
		};
		template<> struct EffectHelper<BS::Leading>
		{
			inline static void mk(BTRChunk& mC, float mX)
			{
				mC.leadingModifier = mX;
				mC.root.mustRefreshGeometry = true;
			}
		};
		template<> struct EffectHelper<BS::HChunkSpacing>
		{
			inline static void mk(BTRChunk& mC, float mX)
			{
				mC.hChunkSpacingModifier = mX;
				mC.root.mustRefreshGeometry = true;
			}
		};
		template<> struct EffectHelper<BS::Pulse>
		{
			inline static auto& mk(BTRChunk& mC, const sf::Color& mColor, float mSpeed, float mMax, float mStart)
			{
				auto& result(mC.mkEffect<BS::ColorFG>(mColor));
				result.setAnimPulse(mSpeed, mMax, mStart);
				return result;
			}
		};
		template<> struct EffectHelper<BS::PulseDef>
		{
			inline static auto& mk(BTRChunk& mC, const sf::Color& mColor)
			{
				return mC.eff<BS::Pulse>(mColor, 0.05f, 110.f, 0.f);
			}
		};
	}
}

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
