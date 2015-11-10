#pragma once

#include "../Common/Common.hpp"
#include "../Payload/Payload.hpp"

namespace nl
{

    // TODO
    namespace Tunnel
    {
        struct Fake
        {
            std::function<void(Payload&)> on_recv;
            std::function<void(Payload&)> on_send;

            bool bind(Port) { return true; }
            bool bound() const noexcept { return true; }

            auto receive_payload(Payload& p)
            {
                on_recv(p);
                return true;
            }

            auto send_payload(Payload& p)
            {
                on_send(p);
                return true;
            }
        };
    }
}
