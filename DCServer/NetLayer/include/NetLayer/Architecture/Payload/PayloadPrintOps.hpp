#pragma once

#include "../../Common/Common.hpp"
#include "./PayloadAddress.hpp"
#include "./Payload.hpp"

namespace nl
{
    namespace Impl
    {
        inline auto& operator<<(std::ostream& mS, const PayloadAddress& mX)
        {
            mS << mX.ip << ":" << mX.port;
            return mS;
        }

        inline auto& operator<<(std::ostream& mS, const Payload& mX)
        {
            mS << mX.address;
            return mS;
        }
    }
}
