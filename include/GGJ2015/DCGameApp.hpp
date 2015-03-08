#ifndef GGJ2015_GAMEAPP
#define GGJ2015_GAMEAPP

#include "../GGJ2015/DCCommon.hpp"

namespace Exp
{
	using namespace ssvs;

	struct BSC_Tracking
	{
		float value;
	};

	class BTRRoot;
	class BTRChunk;
	template<typename T, typename... TArgs> struct BSC_Eff;

	class BSC_StrRef
	{
		friend class BTRChunk;

		private:
			BTRChunk* chunk{nullptr};

		public:
			template<typename T> auto& operator=(T&& mX);
			bool operator!=(const std::string& mX);
	};

	class BTREffect
	{
		friend class BTRRoot;

		public:
			inline virtual void update(FT) noexcept { }
			inline virtual void apply(BTRChunk&) noexcept { }
	};

	class BTRChunk
	{
		friend class BTRRoot;

		private:
			BTRRoot& root;
			BTRChunk* parent{nullptr};
			std::string str;
			std::vector<BTREffect*> childrenEffects;
			std::vector<BTRChunk*> children;
			SizeT idxVBegin, idxVEnd;

			float trackingModifier{0.f};

			template<typename... TArgs> BTRChunk& mkChild(TArgs&&... mArgs);
			template<typename T, typename... TArgs> T& mkEffect(TArgs&&... mArgs);

		public:
			inline BTRChunk(BTRRoot& mRoot) noexcept : root{mRoot} { }

			template<typename TF> void forVertices(TF mFn) noexcept;

			void refreshGeometry() noexcept;
			void refreshEffects() noexcept;

			inline void update(FT mFT) noexcept
			{
				for(auto& e : childrenEffects) e->update(mFT);
				for(auto& c : children) c->update(mFT);
			}

			inline float getTracking() noexcept
			{
				if(parent == nullptr) return 0;
				return parent->getTracking() + parent->trackingModifier;
			}

			template<typename T> void setStr(T&& mX);
			inline const auto& getStr() const noexcept { return str; }

			auto& operator<<(const std::string& mX);
			auto& operator<<(const BSC_Tracking& mX);
			auto& operator<<(const sf::Color& mX);
			auto& operator<<(BSC_StrRef& mX);
			template<typename T, typename... TArgs> auto& operator<<(const BSC_Eff<T, TArgs...>& mX);
	};

	using BTRChunkRecycler = ssvu::MonoRecycler<BTRChunk>;
	using BTRChunkPtr = typename BTRChunkRecycler::PtrType;

	using BTREffectRecycler = ssvu::PolyRecycler<BTREffect>;
	using BTREffectPtr = typename BTREffectRecycler::PtrType;

	struct BTRDrawState
	{
		float xMin, xMax, yMin, yMax;
		SizeT iX, iY, width, height;

		inline void reset(const BitmapFont& mBF) noexcept
		{
			xMin = xMax = yMin = yMax = 0;
			iX = iY = 0;
			width = mBF.getCellWidth();
			height = mBF.getCellHeight();
		}
	};

	class BTREColorFG : public BTREffect
	{
		private:
			sf::Color colorFG;

		public:
			inline BTREColorFG(const sf::Color& mColorFG) noexcept : colorFG{mColorFG} { }

			inline void update(FT) noexcept override { }
			inline void apply(BTRChunk& mX) noexcept override
			{
				mX.forVertices([this](auto mIdx, auto mCount, auto& mV, auto& mVO){ mV.color = colorFG; });
			}
	};

	class BTREWave : public BTREffect
	{
		private:
			float angle;
			float amplitude;
			float repeat;
			float speedMult;

		public:
			inline BTREWave(float mAmplitude = 2.f, float mSpeedMult = 0.1f, float mRepeat = 4.f, float mAngleStart = 0.f)
				: angle{mAngleStart}, amplitude{mAmplitude}, repeat{mRepeat}, speedMult{mSpeedMult} { }

			inline void update(FT mFT) noexcept override { angle = ssvu::wrapRad(angle + mFT * speedMult); }
			inline void apply(BTRChunk& mX) noexcept override
			{
				mX.forVertices([this](auto mIdx, auto mCount, auto& mV, auto& mVO)
				{
					mV.position.y = mVO.position.y + std::sin(angle + (mIdx / repeat)) * amplitude;
				});
			}
	};

	template<typename T, typename... TArgs> struct BSC_Eff
	{
		std::tuple<TArgs...> args;

		inline BSC_Eff(TArgs... mArgs) : args{mArgs...} { }
	};

	template<typename T, typename... TArgs> auto mkEff(TArgs&&... mArgs)
	{
		return BSC_Eff<T, TArgs...>{FWD(mArgs)...};
	}

	class BTRRoot : public sf::Transformable, public sf::Drawable
	{
		friend class BTRChunk;

		private:
			const BitmapFont* bitmapFont{nullptr};
			const sf::Texture* texture{nullptr};
			mutable VertexVector<sf::PrimitiveType::Quads> vertices, verticesOriginal;
			mutable sf::FloatRect bounds, globalBounds;
			mutable bool mustRefreshGeometry{true};

			BTRChunkRecycler chunkRecycler;
			std::vector<BTRChunkPtr> chunkManager;

			BTREffectRecycler effectRecycler;
			std::vector<BTREffectPtr> effectManager;

			BTRChunk* baseChunk{&mkChunk()};
			float alignMult{0.f};

			mutable BTRDrawState bdd;
			mutable std::vector<SizeT> rowCells;

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

			template<typename... TArgs> inline BTRChunk& mkChunk(TArgs&&... mArgs)
			{
				return chunkRecycler.getCreateEmplace(chunkManager, *this, FWD(mArgs)...);
			}

			template<typename T, typename... TArgs> inline T& mkEffect(TArgs&&... mArgs)
			{
				return effectRecycler.getCreateEmplace<T>(effectManager, FWD(mArgs)...);
			}

			inline void refreshGeometryStart() const noexcept
			{
				SSVU_ASSERT(bitmapFont != nullptr);

				rowCells.clear();
				vertices.clear();
				verticesOriginal.clear();
				bdd.reset(*bitmapFont);
			}

			inline void refreshGeometryFinish() const
			{
				// Recalculate bounds
				auto width(bdd.xMax - bdd.xMin);
				bounds = {bdd.xMin, bdd.yMin, width, bdd.yMax - bdd.yMin};
				globalBounds = getTransform().transformRect(bounds);

				// Add current row to `rowCells`, return if its the only one
				if(rowCells.empty()) return;
				rowCells.emplace_back(bdd.iX);

				SizeT lastVIdx{0};

				for(auto rc : rowCells)
				{
					auto vIdx(lastVIdx);

					// Find out the width of the current row
					auto targetVIdx(lastVIdx + rc * 4);
					float maxX{0.f};
					for(; vIdx < targetVIdx; vIdx += 4) maxX = std::max({maxX, vertices[vIdx].position.x, vertices[vIdx + 1].position.x});

					// Apply horizontal alignment
					auto offset(width - maxX);
					for(; lastVIdx < vIdx; ++lastVIdx) vertices[lastVIdx].position.x += offset * alignMult;
				}
			}

			inline void mkVertices(BTRChunk& mChunk) const
			{
				const auto& str(mChunk.str);
				//vertices.reserve(str.size() * 4);

				mChunk.idxVBegin = vertices.size();

				for(const auto& c : str)
				{
					switch(c)
					{
						case L'\t': bdd.iX += 4;											continue;
						case L'\n': ++bdd.iY; rowCells.emplace_back(bdd.iX); bdd.iX = 0;	continue;
						case L'\v': bdd.iY += 4;											continue;
					}

					const auto& rect(bitmapFont->getGlyphRect(c));
					auto spacing(mChunk.getTracking() * bdd.iX);

					auto gLeft(bdd.iX * bdd.width + spacing);			ssvu::clampMax(bdd.xMin, gLeft);
					auto gRight((bdd.iX + 1) * bdd.width + spacing);	ssvu::clampMin(bdd.xMax, gRight);
					auto gTop(bdd.iY * bdd.height);						ssvu::clampMax(bdd.yMin, gTop);
					auto gBottom((bdd.iY + 1) * bdd.height);			ssvu::clampMin(bdd.yMax, gBottom);

					vertices.emplace_back(Vec2f(gLeft, gTop),		sf::Color::White,	Vec2f(rect.left,				rect.top));
					vertices.emplace_back(Vec2f(gRight, gTop),		sf::Color::White,	Vec2f(rect.left + rect.width,	rect.top));
					vertices.emplace_back(Vec2f(gRight, gBottom),	sf::Color::White,	Vec2f(rect.left + rect.width,	rect.top + rect.height));
					vertices.emplace_back(Vec2f(gLeft, gBottom),	sf::Color::White,	Vec2f(rect.left,				rect.top + rect.height));

					++bdd.iX;
				}

				mChunk.idxVEnd = vertices.size();
			}

		public:
			inline BTRRoot() noexcept { }
			inline BTRRoot(const BitmapFont& mBF) noexcept : bitmapFont{&mBF}, texture{&bitmapFont->getTexture()} { }

			inline void clear()
			{
				mustRefreshGeometry = true;
				chunkManager.clear();
				baseChunk = &mkChunk();
			}
			inline void update(FT mFT) noexcept
			{
				baseChunk->update(mFT);
			}

			template<typename T> inline auto& operator<<(T&& mX) { return (*baseChunk) << FWD(mX); }

			inline void setAlign(TextAlign mX) noexcept
			{
				auto newAlignMult(static_cast<float>(static_cast<int>(mX)) * 0.5f);

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

	template<typename TF> inline void BTRChunk::forVertices(TF mFn) noexcept
	{
		for(auto i(idxVBegin); i < idxVEnd; ++i) mFn(idxVEnd - i - 1, idxVEnd - idxVBegin, root.vertices[i], root.verticesOriginal[i]);
	}

	inline void BTRChunk::refreshGeometry() noexcept
	{
		root.mkVertices(*this);
		for(auto& c : children) c->refreshGeometry();
	}
	inline void BTRChunk::refreshEffects() noexcept
	{
		if(parent != nullptr)
			for(auto& e : parent->childrenEffects) e->apply(*this);

		for(auto& c : children) c->refreshEffects();

		/*for(auto& c : children)
		{
			for(auto& e : childrenEffects) e->apply(*c);
			c->refreshEffects();
		}*/
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

	inline auto& BTRChunk::operator<<(const std::string& mX)
	{
		auto& result(mkChild());
		result.str = mX;

		root.mustRefreshGeometry = true;
		return result;
	}

	inline auto& BTRChunk::operator<<(const BSC_Tracking& mX)
	{
		trackingModifier = mX.value;

		root.mustRefreshGeometry = true;
		return *this;
	}

	inline auto& BTRChunk::operator<<(const sf::Color& mX)
	{
		mkEffect<BTREColorFG>(mX);
		return *this;
	}

	template<typename T, typename... TArgs> auto& BTRChunk::operator<<(const BSC_Eff<T, TArgs...>& mX)
	{
		ssvu::explode([this, &mX](TArgs... mXs){ mkEffect<T>(mXs...); }, mX.args);
		return *this;
	}

	template<typename T> inline auto& BSC_StrRef::operator=(T&& mX) { chunk->setStr(FWD(mX)); return *this; }
	inline bool BSC_StrRef::operator!=(const std::string& mX) { return chunk->getStr() != mX; }

	template<typename T> inline void BTRChunk::setStr(T&& mX)
	{
		str = FWD(mX);
		root.mustRefreshGeometry = true;
	}

	inline auto& BTRChunk::operator<<(BSC_StrRef& mX)
	{
		auto& result(mkChild());
		mX.chunk = &result;

		root.mustRefreshGeometry = true;
		return result;
	}
}



namespace ggj
{
	class MenuChoice
	{
		private:
			std::string str;
			ssvu::Func<void()> fn;

		public:
			template<typename T> inline MenuChoice(std::string mStr, T&& mFn)
				: str(std::move(mStr)), fn(FWD(mFn))
			{

			}

			inline const auto& getStr() const noexcept { return str; }
			inline void execFn() { fn(); }
	};

	class MenuState;

	class Menu
	{
		friend class MenuState;

		private:
			ssvu::MonoRecycler<MenuChoice> rcyChoices;
			ssvu::MonoRecycler<MenuState> rcyStates;
			std::vector<MenuState*> stateStack;

		public:
			using RPtrChoice = decltype(rcyChoices)::PtrType;
			using RPtrState = decltype(rcyStates)::PtrType;

		private:


		public:
			inline void go(RPtrState& mX) { stateStack.emplace_back(mX.get()); }
			inline RPtrState mkState() { return rcyStates.create(*this); }

			void executeChoice(int mI);
			template<typename TF> void forCurrentChoices(TF&& mFn);
			inline void back() { stateStack.pop_back(); }
	};

	class MenuState
	{
		friend class Menu;

		private:
			Menu& menu;
			std::array<Menu::RPtrChoice, Constants::maxMenuChoices> choices;

		public:
			inline MenuState(Menu& mMenu) : menu(mMenu) { }

			template<typename... TArgs> inline void addChoice(SizeT mIdx, TArgs&&... mArgs)
			{
				choices[mIdx] = menu.rcyChoices.create(FWD(mArgs)...);
			}

			inline void executeChoice(int mI)
			{
				if(choices[mI] == nullptr) return;
				choices[mI]->execFn();
			}
	};

	inline void Menu::executeChoice(int mI)
	{
		stateStack.back()->executeChoice(mI);
	}

	template<typename TF> void Menu::forCurrentChoices(TF&& mFn)
	{
		// TODO: very generic iteration helper (auto mIdx, auto& mX) ??
		auto& choices(stateStack.back()->choices);
		for(auto i(0u); i < choices.size(); ++i)
		{
			auto& c(choices[i]);
			FWD(mFn)(i, c);
		}
	}


	class GameApp : public Boilerplate::App
	{
		enum class State{Menu, Game};

		private:
			State state{State::Menu};

			Menu menu;

			Menu::RPtrState stMain;
			Menu::RPtrState stPlay;
			Menu::RPtrState stSettings;



			GameSession gs;

			ssvs::BitmapText txtTimer{mkTxtOBBig()}, txtRoom{mkTxtOBBig()}, txtDeath{mkTxtOBBig()},
							txtLog{mkTxtOBSmall()}, txtMode{mkTxtOBSmall()};

			ssvs::BitmapTextRich txtCredits{*getAssets().fontObStroked}, txtRestart{*getAssets().fontObStroked},
				txtMenu{*getAssets().fontObStroked}, txtScores{*getAssets().fontObStroked};
			BP::Str* bpstrRoom;
			BP::Str* bpstrMode;

			std::vector<SlotChoice> slotChoices;
			sf::Sprite dropsModalSprite;
			CreatureStatsDraw csdPlayer;
			Vec2f oldPos;

			BP::Str* sScoreName;
			BP::Str* sScoreBeginner;
			BP::Str* sScoreOfficial;
			BP::Str* sScoreHardcore;
			BP::Str* sScorePlayedGames;
			BP::Str* sScorePlayedTime;

			inline void initInput()
			{
				auto& gState(gameState);

				// TODO: better input management, choose handling type
				gState.addInput({{IK::Escape}}, [this](FT){ if(state != State::Menu) gs.gotoMenu(); }, IT::Once);

				/*
				gState.addInput({{IK::A}}, [this](FT){ gameCamera.pan(-4, 0); });
				gState.addInput({{IK::D}}, [this](FT){ gameCamera.pan(4, 0); });
				gState.addInput({{IK::W}}, [this](FT){ gameCamera.pan(0, -4); });
				gState.addInput({{IK::S}}, [this](FT){ gameCamera.pan(0, 4); });
				gState.addInput({{IK::Q}}, [this](FT){ gameCamera.zoomOut(1.1f); });
				gState.addInput({{IK::E}}, [this](FT){ gameCamera.zoomIn(1.1f); });
				*/

				gState.addInput({{IK::Num1}}, [this](FT){ executeChoice(0); }, IT::Once);
				gState.addInput({{IK::Num2}}, [this](FT){ executeChoice(1); }, IT::Once);
				gState.addInput({{IK::Num3}}, [this](FT){ executeChoice(2); }, IT::Once);
				gState.addInput({{IK::Num4}}, [this](FT){ executeChoice(3); }, IT::Once);
				gState.addInput({{IK::Num5}}, [this](FT){ executeChoice(4); }, IT::Once);
			}

			inline void goMenuState(Menu::RPtrState& mX)
			{
				menu.go(mX);
				recreateTxtMenu();
			}

			inline void goMenuBack()
			{
				menu.back();
				recreateTxtMenu();
			}

			inline const auto& getModeStrArray()
			{
				static auto array(ssvu::mkArray
				(
					"Beginner mode",
					"Official mode",
					"Hardcore mode"
				));

				return array;
			}

			inline void initMenu()
			{
				stMain = menu.mkState();
				stMain->addChoice(0, "Play game", [this]{ goMenuState(stPlay); });
				stMain->addChoice(1, "Settings", [this]{ goMenuState(stSettings); });
				stMain->addChoice(4, "Exit game", [this]{ stop(); });

				stPlay = menu.mkState();
				stPlay->addChoice(0, getModeStrArray()[0], [this]{ gotoGame(GameSession::Mode::Beginner); });
				stPlay->addChoice(1, getModeStrArray()[1], [this]{ gotoGame(GameSession::Mode::Official); });
				stPlay->addChoice(2, getModeStrArray()[2], [this]{ gotoGame(GameSession::Mode::Hardcore); });
				stPlay->addChoice(4, "Go back", [this]{ goMenuBack(); });

				goMenuState(stMain);
			}

			inline void gotoMenu()
			{
				gs.gotoMenu();
				state = State::Menu;

				sScoreName->setStr(gs.pd.name);

				sScoreBeginner->setStr(ssvu::toStr(gs.pd.scoreBeginner));
				sScoreOfficial->setStr(ssvu::toStr(gs.pd.scoreOfficial));
				sScoreHardcore->setStr(ssvu::toStr(gs.pd.scoreHardcore));

				sScorePlayedGames->setStr(ssvu::toStr(gs.pd.gamesPlayed));
				sScorePlayedTime->setStr(ssvu::toStr(gs.pd.timePlayed));
			}

			inline void gotoGame(GameSession::Mode mMode)
			{
				state = State::Game;
				gs.mode = mMode;
				gs.restart();
			}

			inline void executeChoiceDead(int mI)
			{
				if(mI == 0) gotoMenu();
				else if(mI == 1) gs.restart();
			}

			inline void executeChoicePlaying(int mI)
			{
				if(mI == 4)
				{
					// TODO: return
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

			inline void executeChoice(int mI)
			{
				if(state == State::Menu) menu.executeChoice(mI);
				else if(gs.state == GameSession::State::Playing) executeChoicePlaying(mI);
				else if(gs.state == GameSession::State::Dead) executeChoiceDead(mI);
			}



			inline const auto& getModeStr()
			{
				return getModeStrArray()[static_cast<int>(gs.mode)];
			}

			inline void updatePlaying(FT mFT)
			{
				if(gs.currentDrops != nullptr) gs.currentDrops->update(mFT);

				csdPlayer.update(mFT);
				for(auto& c : gs.choices) if(c != nullptr) c->update(mFT);

				if(gs.gd.timerEnabled) gs.timer -= mFT;

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

					// auto third(gameWindow.getWidth() / 5.f);

					txtTimer.setString(gs.gd.timerEnabled ? "00:" + gts : "XX:XX");
					txtRoom.setString(ssvu::toStr(gs.roomNumber));

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

			inline void updateMenu(FT mFT)
			{
				txtMenu.update(mFT);
				//txtSelectMode.update(mFT);
				txtScores.update(mFT);
				tr.update(mFT);
			}

			inline void updateDead(FT mFT)
			{
				bpstrRoom->setStr(ssvu::toStr(gs.roomNumber));
				bpstrMode->setStr(getModeStr());

				txtRestart.update(mFT);
			}

			inline void update(FT mFT)
			{
				txtCredits.update(mFT);
				gameCamera.update<float>(mFT);
				reftestC += mFT;

				if(reftestC > 50)
				{
					if(reftest != "everyone!")
					{
						reftest = "everyone!";
					}
					else
					{
						reftest = "Hello";
					}

					reftestC = 0;
				}


				if(gs.deathTextTime > 0) gs.deathTextTime -= mFT;

				if(state == State::Menu) updateMenu(mFT);
				else if(gs.state == GameSession::State::Playing) updatePlaying(mFT);
				else if(gs.state == GameSession::State::Dead) updateDead(mFT);

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



			inline void drawPlaying()
			{
				// Panel: timer
				sf::Sprite p1{*getAssets().panelsmall};
				ssvs::setOrigin(p1, ssvs::getLocalNW);
				p1.setPosition(10, 5);
				render(p1);

				ssvs::setOrigin(txtTimer, ssvs::getLocalCenter);
				txtTimer.setPosition(ssvs::getGlobalCenter(p1) + Vec2f(0, 3));



				// Panel: room
				sf::Sprite p2{*getAssets().panelsmall};
				ssvs::setOrigin(p2, ssvs::getLocalNE);
				p2.setPosition(320 - 10, 5);
				render(p2);

				ssvs::setOrigin(txtRoom, ssvs::getLocalCenter);
				txtRoom.setPosition(ssvs::getGlobalCenter(p2) + Vec2f(0, 3));



				// Panel: log
				sf::Sprite p3{*getAssets().panellog};
				ssvs::setOrigin(p3, ssvs::getLocalSW);
				p3.setPosition(70 + 12, 240 - 5);
				render(p3);



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
							sc.txtNum.setString("1");
							sc.txtStr.setString("Back");
						}
						else if(gs.currentDrops->has(i - 1))
						{
							gs.currentDrops->drops[i -1]->draw(gameWindow, sc.shape.getPosition(), sc.getCenter());
							sc.txtNum.setString(ssvu::toStr(i + 1));
							sc.txtStr.setString("Pickup");
						}
						else
						{
							sc.txtNum.setString("");
							sc.txtStr.setString("");
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

						sc.txtNum.setString(gc == nullptr ? "" : ssvu::toStr(i + 1));
						sc.txtStr.setString(gc == nullptr ? "Blocked" : gc->getChoiceStr());
						sc.sprite.setTexture(gc == nullptr ? *getAssets().slotChoiceBlocked : *getAssets().slotChoice);
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

			inline void drawDead()
			{
				txtDeath.setString("You have perished.");

				txtDeath.setPosition(320 / 2.f, 80);
				txtRestart.setPosition(320 / 2.f, 120);

				render(txtDeath);
				render(txtRestart);

				txtDeath.setColor(sf::Color(255, 255, 255, 255 - static_cast<unsigned char>(gs.deathTextTime)));
			}

			inline void drawMenu()
			{
				txtDeath.setString("DELVER'S CHOICE");
				txtDeath.setColor(sf::Color(255, 255, 255, 255));

				txtDeath.setPosition(320 / 2.f, 30);
				txtMenu.setPosition(320 / 2.f, 70);
				txtScores.setPosition(320 - 5, 240 - 5);
				txtCredits.setPosition(5, 240 - 5);

				render(txtDeath);
				render(txtMenu);
				render(txtScores);
				render(txtCredits);
			}

			inline void draw()
			{
				ssvs::setOrigin(txtDeath, ssvs::getLocalCenter);
				ssvs::setOrigin(txtRestart, ssvs::getLocalCenter);
				ssvs::setOrigin(txtMenu, ssvs::getLocalCenter);
				ssvs::setOrigin(txtScores, ssvs::getLocalSE);
				ssvs::setOrigin(txtCredits, ssvs::getLocalSW);

				ssvs::setOrigin(tr, ssvs::getLocalCenter);
				tr.setPosition(100, 130);
				render(tr);

				if(state == State::Menu)
				{
					drawMenu();
					return;
				}

				gameCamera.apply();

				if(gs.state == GameSession::State::Playing || gs.deathTextTime > 0)
					drawPlaying();

				gameCamera.unapply();


				if(gs.state == GameSession::State::Dead) drawDead();
			}

			inline auto& mkTP(ssvs::BitmapTextRich& mTxt, const sf::Color& mC)
			{
				auto& temp(mTxt.template mk<BP::ClFG>(mC));
				temp.setAnimPulse(0.05f, 100);
				return temp;
			}

			inline void recreateTxtMenu()
			{
				txtMenu.clear();
				txtMenu.setAlign(ssvs::TextAlign::Center);
				txtMenu	<< txtMenu.mk<BP::Trk>(-3);

				menu.forCurrentChoices([this](auto mIdx, auto& mC)
				{
					if(mC == nullptr)
					{
						txtMenu << "\n";
					}
					else
					{
						txtMenu << mkTP(txtMenu, sfc::Red) << ssvu::toStr(mIdx + 1) << ". " << sfc::White << mC->getStr() << "\n";
					}
				});
			}

		public:
			Exp::BTRRoot tr{*getAssets().fontObStroked};
			Exp::BSC_StrRef reftest;
			float reftestC;

			inline GameApp(ssvs::GameWindow& mGameWindow) : Boilerplate::App{mGameWindow}
			{
				tr.setAlign(ssvs::TextAlign::Center);
				tr << Exp::mkEff<Exp::BTREWave>(1.5f, 0.03f) << "Testing rich text...\n" << Exp::BSC_Tracking{-3} << sfc::Red << "Here it goes: " << sfc::Cyan << reftest << Exp::BSC_Tracking{+1} << "\n:D";

				txtCredits	<< txtCredits.mk<BP::Trk>(-3)
							<< mkTP(txtCredits, sfc::White) << "Global Game Jam 2015\n"
							<< sfc::White << "Developer: " << sfc::Red << "Vittorio Romeo\n"
							<< sfc::White << "2D Artist: " << sfc::Red << "Vittorio Romeo\n"
							<< sfc::White << "Audio: " << sfc::Green << "Nicola Bombaci\n"
							<< sfc::White << "Designer: " << sfc::Yellow << "Sergio Zavettieri\n"
							<< sfc::White << "Additional help: " << sfc::Magenta << "Davide Iuffrida\n"
							<< sfc::Cyan << "http://vittorioromeo.info\nhttp://nicolabombaci.com";

				bpstrRoom = &txtRestart.mk<BP::Str>();
				bpstrMode = &txtRestart.mk<BP::Str>();
				txtRestart	<< txtRestart.mk<BP::Trk>(-3)
							<< sfc::White << "Press " << mkTP(txtRestart, sfc::Red) << "1 " << sfc::White << "for menu.\n"
							<< sfc::White << "Press " << mkTP(txtRestart, sfc::Red) << "2 " << sfc::White << "to restart.\n"
							<< sfc::White << "You reached room " << sfc::Green << *bpstrRoom << sfc::White << ".\n"
							<< sfc::Cyan << "(" << *bpstrMode << ")";

				sScoreName = &txtScores.mk<BP::Str>();
				sScoreBeginner = &txtScores.mk<BP::Str>();
				sScoreOfficial = &txtScores.mk<BP::Str>();
				sScoreHardcore = &txtScores.mk<BP::Str>();
				sScorePlayedGames = &txtScores.mk<BP::Str>();
				sScorePlayedTime = &txtScores.mk<BP::Str>();

				txtScores.setAlign(ssvs::TextAlign::Right);
				txtScores	<< txtScores.mk<BP::Trk>(-3)
							<< sfc::White << "Welcome, " << mkTP(txtScores, sfc::Cyan) << *sScoreName << sfc::White << "!\n\n"
							<< mkTP(txtScores, sfc::Red) << "High scores:\n"
							<< sfc::White << "Beginner: " << mkTP(txtScores, sfc::Green) << *sScoreBeginner << "\n"
							<< sfc::White << "Official: " << mkTP(txtScores, sfc::Green) << *sScoreOfficial << "\n"
							<< sfc::White << "Hardcore: " << mkTP(txtScores, sfc::Green) << *sScoreHardcore << "\n\n"
							<< mkTP(txtScores, sfc::Red) << "Statistics:\n"
							<< sfc::White << "Games played: " << mkTP(txtScores, sfc::Green) << *sScorePlayedGames << "\n"
							<< sfc::White << "Time played: " << mkTP(txtScores, sfc::Green) << *sScorePlayedTime << "\n";


				for(int i{0}; i < 4; ++i) slotChoices.emplace_back(i);

				gameState.onUpdate += [this](FT mFT){ update(mFT); };
				gameState.onDraw += [this]{ draw(); };

				dropsModalSprite.setTexture(*getAssets().dropsModal);
				dropsModalSprite.setPosition(10, 40);

				txtLog.setPosition(Vec2f{75 + 12, 180});

				initInput();
				initMenu();

				oldPos = gameCamera.getCenter();

				gotoMenu();
			}
	};

}

#endif

// TODO: btre effects grouping not working
