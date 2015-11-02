#pragma once

#include "../Common/Common.hpp"
#include "./PayloadAddress.hpp"

namespace nl
{
    // Combination of data, ip and port.
    struct Payload
    {
        PAddress address;
        PcktBuf data;

        Payload() = default;
        Payload(const PAddress& mAddress) noexcept : address{mAddress} {}

        template <typename TData>
        Payload(const PAddress& mAddress, TData&& mData)
            : address{mAddress}, data(FWD(mData))
        {
        }
    };
}
