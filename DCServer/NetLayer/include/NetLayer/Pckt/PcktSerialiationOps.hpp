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


// TODO:
template <typename... Ts>
inline auto& operator<<(std::ostream& o, const nl::Impl::Pckt<Ts...>& mX)
{
    return o << mX.fields;
}

template <typename... Ts>
inline auto& operator<<(std::ostream& o, const ssvu::Tpl<Ts...>& mX)
{
    ssvu::tplFor(
        [&](const auto& x)
        {
            o << x;
        },
        mX);

    return o;
}


// TODO:
namespace nl
{
    template <typename T, typename... Ts>
    auto make_pckt(Ts&&... fields)
    {
        T res(nl::init_fields{}, FWD(fields)...);
        return res;
    }
}
