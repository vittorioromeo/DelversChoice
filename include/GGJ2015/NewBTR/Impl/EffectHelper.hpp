#ifndef GGJ2015_NEWBTR_IMPL_EFFECTHELPER
#define GGJ2015_NEWBTR_IMPL_EFFECTHELPER

#include "../../../GGJ2015/Common.hpp"

namespace Exp
{
	using namespace ssvs;

	namespace Impl
	{
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

#endif
