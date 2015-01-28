#ifndef GGJ2015_COMMON
#define GGJ2015_COMMON

#include <SSVStart/SSVStart.hpp>

namespace ggj 
{
	using SizeT = ssvu::SizeT;
	template<typename T> using Vec2 = ssvs::Vec2<T>;
	template<typename T, typename TD = ssvu::DefDel<T>> using UPtr = ssvs::UPtr<T, TD>;
	using Vec2i = ssvs::Vec2i;
	using Vec2f = ssvs::Vec2f;
	using Vec2u = ssvs::Vec2u;
	using FT = ssvu::FT;
	using Trigger = ssvs::Input::Trigger;
}

#endif