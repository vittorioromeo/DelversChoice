#pragma once

#include "../Common/Common.hpp"
#include "../Payload/Payload.hpp"

namespace nl
{
    namespace Impl
    {
        namespace Tunnel
        {
            class UDPSckt
            {
            private:
                ScktUdp sckt;

            public:
                template <typename... Ts>
                UDPSckt(Ts&&... xs)
                    : sckt{FWD(xs)...}
                {
                    sckt.setBlocking(false);
                }

                ~UDPSckt() { sckt.unbind(); }

                bool bind(Port x) { return sckt.bind(x) == sf::Socket::Done; }

                auto receive_payload(Payload& p)
                {
                    return scktRecv(sckt, p) == sf::Socket::Done;
                }

                auto send_payload(Payload& p)
                {
                    return scktSend(sckt, p) == sf::Socket::Done;
                }
            };
        }
    }
}
