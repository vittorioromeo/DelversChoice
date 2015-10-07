#pragma once

#include "../../Common/Common.hpp"

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
        };
    }
}
