#ifndef GGJ2015_NEWBTR_IMPL_BTREWAVE
#define GGJ2015_NEWBTR_IMPL_BTREWAVE

#include "../../../GGJ2015/Common.hpp"

namespace Exp
{
	using namespace ssvs;

	namespace Impl
	{
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
	}
}

#endif
