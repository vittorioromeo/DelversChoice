#pragma once

#include "../Common/Common.hpp"

namespace nl
{
    struct PAddress
    {
        IpAddr ip;
        Port port;

        PAddress() = default;
        PAddress(const IpAddr& mIp, Port mPort) noexcept : ip{mIp}, port{mPort}
        {
        }

        inline bool operator==(const PAddress& rhs) noexcept
        {
            return ip == rhs.ip && port == rhs.port;
        }

        inline bool operator!=(const PAddress& rhs) noexcept
        {
            return !(*this == rhs);
        }
    };
}
