#ifndef GGJ2015_NEWBTR_IMPL_BTREFFECT
#define GGJ2015_NEWBTR_IMPL_BTREFFECT

#include <SSVUtils/MemoryManager/MemoryManager.hpp>
#include "SSVStart/Global/Typedefs.hpp"
#include "SSVStart/BitmapText/Impl/BitmapFont.hpp"

namespace ssvs
{
	namespace Impl
	{
		class BTREffect
		{
			friend class BTRRoot;

			public:
				inline virtual void update(FT) noexcept { }
				inline virtual void apply(BTRChunk&) noexcept { }
		};
	}
}

#endif
