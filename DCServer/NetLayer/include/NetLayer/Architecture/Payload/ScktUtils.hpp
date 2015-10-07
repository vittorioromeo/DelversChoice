#pragma once

#include "../../Common/Common.hpp"
#include "./Payload.hpp"

namespace nl
{
    namespace Impl
    {
        template <typename T>
        inline auto scktRecv(T& mSckt, Payload& mBuffer) noexcept
        {
            return mSckt.receive(
                mBuffer.data, mBuffer.address.ip, mBuffer.address.port);
        }

        template <typename T>
        inline auto scktSend(T& mSckt, Payload& mBuffer) noexcept
        {
            return mSckt.send(
                mBuffer.data, mBuffer.address.ip, mBuffer.address.port);
        }
    }
}
