#ifndef GGJ2015_NEWBTR_IMPL_BTRECOLORFG
#define GGJ2015_NEWBTR_IMPL_BTRECOLORFG

#include "../../../GGJ2015/Common.hpp"

namespace Exp
{
	using namespace ssvs;

	namespace Impl
	{
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

				inline void setColorFG(const sf::Color& mX) noexcept { colorFG = mX; }
		};
	}
}

#endif
