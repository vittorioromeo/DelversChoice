#ifndef GGJ2015_NEWBTR_IMPL_EFFECTHELPER
#define GGJ2015_NEWBTR_IMPL_EFFECTHELPER

#include <SSVUtils/MemoryManager/MemoryManager.hpp>
#include "SSVStart/Global/Typedefs.hpp"
#include "SSVStart/BitmapText/Impl/BitmapFont.hpp"

#include "../../NewBTR/Impl/BTREffect.hpp"
#include "../../NewBTR/Impl/BTRChunk.hpp"
#include "../../NewBTR/Impl/BTREWave.hpp"
#include "../../NewBTR/Impl/BTREColor.hpp"
#include "../../NewBTR/Impl/BTRDrawState.hpp"
#include "../../NewBTR/Impl/BTRRoot.hpp"
#include "../../NewBTR/Impl/BS.hpp"
#include "../../NewBTR/Impl/BTRChunk.inl"

namespace ssvs
{
	namespace BTR
	{
		namespace Impl
		{
			template<typename T> struct EffectHelper
			{
				template<typename... TArgs> inline static auto& mk(BTRChunk& mC, TArgs&&... mArgs)
				{
					return mC.mkEffect<T>(FWD(mArgs)...);
				}
			};
			template<> struct EffectHelper<BTR::Tracking>
			{
				inline static void mk(BTRChunk& mC, float mX)
				{
					mC.trackingModifier = mX;
					mC.root.mustRefreshGeometry = true;
				}
			};
			template<> struct EffectHelper<BTR::Leading>
			{
				inline static void mk(BTRChunk& mC, float mX)
				{
					mC.leadingModifier = mX;
					mC.root.mustRefreshGeometry = true;
				}
			};
			template<> struct EffectHelper<BTR::HChunkSpacing>
			{
				inline static void mk(BTRChunk& mC, float mX)
				{
					mC.hChunkSpacingModifier = mX;
					mC.root.mustRefreshGeometry = true;
				}
			};
			template<> struct EffectHelper<BTR::Pulse>
			{
				inline static auto& mk(BTRChunk& mC, const sf::Color& mColor, float mSpeed, float mMax, float mStart)
				{
					auto& result(mC.mkEffect<BTR::Color>(mColor));
					result.setAnimPulse(mSpeed, mMax, mStart);
					return result;
				}
			};
			template<> struct EffectHelper<BTR::PulseDef>
			{
				inline static auto& mk(BTRChunk& mC, const sf::Color& mColor)
				{
					return mC.eff<BTR::Pulse>(mColor, 0.05f, 110.f, 0.f);
				}
			};
		}
	}
}

#endif
