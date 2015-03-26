#ifndef GGJ2015_NEWBTR_IMPL_BS
#define GGJ2015_NEWBTR_IMPL_BS

#include "../../../GGJ2015/Common.hpp"

namespace Exp
{
	using namespace ssvs;

	namespace BS
	{
		struct Tracking { };
		struct Leading { };
		struct HChunkSpacing { };
		struct Pulse { };
		struct PulseDef { };
		using ColorFG = Impl::BTREColorFG;
		using Wave = Impl::BTREWave;
		using Chunk = Impl::BTRChunk;
	}

	using BitmapTextRich = Impl::BTRRoot;
}

#endif
