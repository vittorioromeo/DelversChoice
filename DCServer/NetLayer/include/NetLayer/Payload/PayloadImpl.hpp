#pragma once

#include "../Common/Common.hpp"
#include "./PayloadAddress.hpp"

namespace nl
{
    namespace Impl
    {
        // Combination of data, ip and port.
        struct Payload
        {
            PayloadAddress address;
            PcktBuf data;

            Payload() = default;
            Payload(const PayloadAddress& mAddress) noexcept : address{mAddress}
            {
            }

            template <typename TData>
            Payload(const PayloadAddress& mAddress, TData&& mData)
                : address{mAddress}, data(FWD(mData))
            {
            }
        };
    }
}
