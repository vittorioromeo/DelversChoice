#pragma once

#include "../Common/Common.hpp"
#include "./SerializationOps.hpp"
#include "../Pckt/PcktSerialiationOps.hpp"

namespace nl
{
    template <typename... Ts>
    void serialize(PcktBuf& p, Ts&&... xs)
    {
        ssvu::forArgs(
            [&p](auto&& x)
            {
                p << FWD(x);
            },
            FWD(xs)...);

        //  ::serialize(p, FWD(xs)...);
    }


    template <typename... Ts>
    void deserialize(PcktBuf& p, Ts&... xs)
    {
        ssvu::forArgs(
            [&p](auto& x)
            {
                p >> x;
            },
            FWD(xs)...);
    }

    template <typename... Ts>
    auto make_serialized(Ts&&... xs)
    {
        PcktBuf p;
        serialize(p, FWD(xs)...);

        return p;
    }

    template <typename T>
    auto make_deserialized(PcktBuf& p)
    {
        T out;
        deserialize<T>(p, out);
        return out;
    }
}
