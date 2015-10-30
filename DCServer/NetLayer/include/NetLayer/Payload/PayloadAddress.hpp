#pragma once

#include "../Common/Common.hpp"

namespace nl
{
    namespace Impl
    {
        struct PayloadAddress
        {
            IpAddr ip;
            Port port;

            PayloadAddress() = default;
            PayloadAddress(const IpAddr& mIp, Port mPort) noexcept : ip{mIp},
                                                                     port{mPort}
            {
            }

            inline bool operator==(const PayloadAddress& rhs) noexcept
            {
                return ip == rhs.ip && port == rhs.port;
            }

            inline bool operator!=(const PayloadAddress& rhs) noexcept
            {
                return !(*this == rhs);
            }
        };
    }
}
