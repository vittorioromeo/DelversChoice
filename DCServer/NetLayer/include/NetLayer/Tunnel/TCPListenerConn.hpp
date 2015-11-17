#pragma once

#include "../Common/Common.hpp"
#include "../Payload/Payload.hpp"
#include "../Utils/Retry.hpp"

namespace nl
{
    namespace Tunnel
    {
        class TCPListenerConn
        {
        private:
            sf::TcpSocket _sckt;
            int _ctr{0};

        public:
            TCPListenerConn() { _sckt.setBlocking(false); }

            TCPListenerConn(const TCPListenerConn&) = delete;
            TCPListenerConn& operator=(const TCPListenerConn&) = delete;

            TCPListenerConn(TCPListenerConn&&) = default;
            TCPListenerConn& operator=(TCPListenerConn&&) = default;

            void recv_dummy_or_inc_ctr()
            {
                std::size_t dummy;
                if(_sckt.receive(&dummy, 0, dummy) == sf::Socket::Disconnected)
                {
                    _ctr += 1;
                }
                else
                {
                    reset_ctr();
                }
            }

            void reset_ctr() { _ctr = 0; }
            auto ctr() const noexcept { return _ctr; }

            auto port() const noexcept { return _sckt.getRemotePort(); }
            auto ip() const noexcept { return _sckt.getRemoteAddress(); }

            auto& sckt() { return _sckt; }
        };
    }
}
