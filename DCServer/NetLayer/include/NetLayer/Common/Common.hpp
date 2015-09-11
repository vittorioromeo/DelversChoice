#pragma once

#include "./API.hpp"
#include "./Deps.hpp"
#include "./Typedefs.hpp"

#if (1)

	#define NL_DEBUGLO() \
		::ssvu::lo() << "\nDEBUG: " 

#else

	namespace nl
	{
		namespace Impl
		{
			struct FakeStream
			{
				template<typename T> FakeStream& operator<<(T&&) { return *this; }
			};
		}
	}

	#define NL_DEBUGLO() \
		::nl::Impl::FakeStream{} 

#endif
