#ifndef GGJ2015_GAMEAPP
#define GGJ2015_GAMEAPP

#include "../GGJ2015/DCCommon.hpp"

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

				if(state == State::Menu)
				{
					drawMenu();
					return;
				}

				gameCamera.apply();

				if(gs.state == GameSession::State::Playing || gs.deathTextTime > 0)
					drawPlaying();

				gameCamera.unapply();

				txtCredits.setPosition(5, 240 - 5);
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
			inline GameApp(ssvs::GameWindow& mGameWindow) : Boilerplate::App{mGameWindow}
			{
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
