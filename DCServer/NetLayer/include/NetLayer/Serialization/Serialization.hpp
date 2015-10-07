#pragma once

#include <array>
#include "../Common/Common.hpp"
#include "../Architecture/Architecture.hpp"
#include "../Pckt/Pckt.hpp"
#include "./SerializationOps.hpp"
#include "/home/vittorioromeo/OHWorkspace/cppcon2015/Other/Other.hpp"

namespace nl
{
    template <typename T>
    void serialize(nl::PcktBuf& p, T&& x)
    {
        p << FWD(x);
    }

    template <typename T>
    void deserialize(nl::PcktBuf& p, T& x)
    {
        p >> x;
    }
}
