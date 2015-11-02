#pragma once

#include "../Common/Common.hpp"
#include "../Pckt/Pckt.hpp"

template <typename... Ts>
inline auto& operator<<(nl::PcktBuf& mP, const nl::Impl::Pckt<Ts...>& mX)
{
    return mP << mX.fields;
}

template <typename... Ts>
inline auto& operator>>(nl::PcktBuf& mP, nl::Impl::Pckt<Ts...>& mX)
{
    return mP >> mX.fields;
}
