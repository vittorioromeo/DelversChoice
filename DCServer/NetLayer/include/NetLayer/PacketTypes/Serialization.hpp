#pragma once

#include "./PacketTypes.hpp"

template<typename T> inline auto& operator<<(nl::PcktBuf& mP, const std::vector<T>& mX)
{
	mP << ssvu::toNum<nl::UInt32>(mX.size());
	for(const auto& x : mX) mP << x;

	return mP;
}
template<typename T> inline auto& operator>>(nl::PcktBuf& mP, std::vector<T>& mX)
{
	// Get size
	nl::UInt32 sz;
	mP >> sz;

	mX.clear();
	mX.resize(sz);

	for(decltype(sz) i(0); i < sz; ++i)
		mP >> mX[i];

	return mP;
}

template<typename... Ts> inline auto& operator<<(nl::PcktBuf& mP, const ssvu::Tpl<Ts...>& mX)
{
	ssvu::tplFor([&mP, &mX](const auto& mV){ mP << mV; }, mX);
	return mP;
}
template<typename... Ts> inline auto& operator>>(nl::PcktBuf& mP, ssvu::Tpl<Ts...>& mX)
{
	ssvu::tplFor([&mP, &mX](auto& mV){ mP >> mV; }, mX);
	return mP;
}

template<typename... Ts> inline auto& operator<<(nl::PcktBuf& mP, const nl::Pckt<Ts...>& mX)
{
	return mP << mX.fields;
}
template<typename... Ts> inline auto& operator>>(nl::PcktBuf& mP, nl::Pckt<Ts...>& mX)
{
	return mP >> mX.fields;
}
