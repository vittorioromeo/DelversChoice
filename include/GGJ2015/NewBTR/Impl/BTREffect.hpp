#ifndef GGJ2015_NEWBTR_IMPL_BTREFFECT
#define GGJ2015_NEWBTR_IMPL_BTREFFECT

#include "../../../GGJ2015/Common.hpp"

namespace Exp
{
	using namespace ssvs;

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
