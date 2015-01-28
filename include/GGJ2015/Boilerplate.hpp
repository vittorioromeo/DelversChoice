#ifndef GGJ2015_BOILERPLATE
#define GGJ2015_BOILERPLATE

namespace Boilerplate
{
	class App
	{
		protected:
			using IK = ssvs::KKey;
			using IB = ssvs::MBtn;
			using IT = ssvs::Input::Type;
			using IM = ssvs::Input::Mode;

			ssvs::GameState gameState;
			ssvs::Camera gameCamera;
			ssvs::GameWindow& gameWindow;

		public:
			inline App(ssvs::GameWindow& mGameWindow) : gameCamera{mGameWindow, 1.f}, gameWindow{mGameWindow} { }

			inline void stop() noexcept	{ return gameWindow.stop(); }

			template<typename... TArgs> inline void render(TArgs&&... mArgs)
			{
				gameWindow.draw(ssvu::fwd<TArgs>(mArgs)...);
			}

			inline auto& getGameState() noexcept				{ return gameState; }
			inline const auto& getGameState() const noexcept	{ return gameState; }
			inline auto& getGameCamera() noexcept				{ return gameCamera; }
			inline const auto& getGameCamera() const noexcept	{ return gameCamera; }
			inline auto& getGameWindow() noexcept				{ return gameWindow; }
			inline const auto& getGameWindow() const noexcept	{ return gameWindow; }
	};

	template<typename T> class AppRunner
	{
		private:
			ssvs::GameWindow gameWindow;
			ssvu::AlignedStorageFor<T> app;

		public:
			inline AppRunner(const std::string& mTitle, ssvu::SizeT mWidth, ssvu::SizeT mHeight)
			{
				gameWindow.setTitle(mTitle);
				gameWindow.setTimer<ssvs::TimerStatic>(0.5f, 0.5f);
				gameWindow.setSize(mWidth, mHeight);
				gameWindow.setFullscreen(false);
				gameWindow.setFPSLimited(true);
				gameWindow.setMaxFPS(200);
				gameWindow.setPixelMult(2);

				new(&app) T{gameWindow};

				gameWindow.setGameState(reinterpret_cast<T&>(app).getGameState());
				gameWindow.run();
			}

			inline ~AppRunner() { reinterpret_cast<T&>(app).~T(); }
	};
}

#endif
