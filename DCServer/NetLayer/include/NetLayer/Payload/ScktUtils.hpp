#pragma once

#include "../Common/Common.hpp"
#include "./PayloadImpl.hpp"

namespace nl
{
    namespace Impl
    {
        template <typename T>
        inline auto scktRecv(T& sckt, Payload& p) noexcept
        {
            return sckt.receive(p.data, p.address.ip, p.address.port);
        }

        template <typename T>
        inline auto scktSend(T& sckt, Payload& p) noexcept
        {
            return sckt.send(p.data, p.address.ip, p.address.port);
        }
    }
}
