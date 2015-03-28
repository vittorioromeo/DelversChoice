#ifndef GGJ2015_NEWBTR_IMPL_FWD
#define GGJ2015_NEWBTR_IMPL_FWD

#include <SSVUtils/MemoryManager/MemoryManager.hpp>
#include "SSVStart/Global/Typedefs.hpp"
#include "SSVStart/BitmapText/Impl/BitmapFont.hpp"

namespace ssvs
{
	namespace BTR
	{
		namespace Impl
		{
			class BTRRoot;
			class BTRChunk;
			template<typename> struct EffectHelper;
		}

		template<typename T> class Ptr
		{
			private:
				T* ptr;

			public:
				inline Ptr() noexcept : ptr{nullptr} { }
				inline Ptr(T* mX) noexcept : ptr{*mX} { }

				inline auto& operator=(T* mX) noexcept { ptr = mX; return *this; }

				inline auto& operator*() noexcept { SSVU_ASSERT(ptr != nullptr); return *ptr; }
				inline const auto& operator*() const noexcept { SSVU_ASSERT(ptr != nullptr); return *ptr; }
				inline T* operator->() noexcept { SSVU_ASSERT(ptr != nullptr); return ptr; }
				inline T* operator->() const noexcept { SSVU_ASSERT(ptr != nullptr); return ptr; }
		};
	}
}

#endif
