#pragma once

#include "../Common/Common.hpp"
#include "./SerializationOps.hpp"

namespace nl
{
    template <typename... Ts>
    void serialize(nl::PcktBuf& p, Ts&&... xs)
    {
        ssvu::forArgs(
            [&p](auto&& x)
            {
                p << FWD(x);
            },
            FWD(xs)...);
    }

    template <typename... Ts>
    void deserialize(nl::PcktBuf& p, Ts&... xs)
    {
        ssvu::forArgs(
            [&p](auto&& x)
            {
                p >> x;
            },
            FWD(xs)...);
    }

    template <typename... Ts>
    auto make_serialized(Ts&&... xs)
    {
        nl::PcktBuf p;
        serialize(p, FWD(xs)...);

        return p;
    }

    template <typename T>
    auto make_deserialized(nl::PcktBuf& p)
    {
        T out;
        deserialize<T>(p, out);
        return out;
    }
}
