#pragma once

#include "../Common/Common.hpp"
#include "../Payload/Payload.hpp"
#include "../Utils/Retry.hpp"

namespace nl
{
    namespace Tunnel
    {
        class TCPSckt
        {
        private:
            ScktTcp sckt;

        public:
            TCPSckt(const TCPSckt&) = delete;
            TCPSckt& operator=(const TCPSckt&) = delete;

            TCPSckt(TCPSckt&&) = default;
            TCPSckt& operator=(TCPSckt&&) = default;

            template <typename... Ts>
            TCPSckt(Ts&&... xs)
                : sckt{FWD(xs)...}
            {
                sckt.setBlocking(false);
            }

            auto bound() const noexcept { return sckt.getLocalPort() != 0; }

            ~TCPSckt() { sckt.disconnect(); }

            bool bind(IpAddr ip, Port port, sf::Time timeout = sf::Time::Zero)
            {
                return sckt.connect(ip, port, timeout) == sf::Socket::Done;
            }

            auto receive_payload(Payload& p)
            {
                return sckt.receive(p.data) == sf::Socket::Done;
            }

            auto send_payload(Payload& p)
            {
                return sckt.send(p.data) == sf::Socket::Done;
            }
        };
    }
}
