#pragma once

#include "../Common/Common.hpp"
#include "./PayloadAddress.hpp"
#include "./PayloadImpl.hpp"

namespace nl
{
    inline auto& operator<<(std::ostream& mS, const PAddress& mX)
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
