#pragma once

#include "../Common/Common.hpp"
#include "../Pckt/Pckt.hpp"

template <typename... Ts>
inline auto& operator<<(std::ostream& o, const nl::Impl::Pckt<Ts...>& mX)
{
    return o << mX.fields;
}

template <typename... Ts>
inline auto& operator<<(std::ostream& o, const std::tuple<Ts...>& mX)
{
    ssvu::tplFor(
        [&](const auto& x)
        {
            o << x;
        },
        mX);

    return o;
}
