#pragma once

#include "../Common/Common.hpp"
#include "../Architecture/ThreadSafeQueue.hpp"
#include "../Architecture/Payload.hpp"


namespace nl
{
    namespace Impl
    {
        // TODO
        namespace Tunnel
        {
            struct Fake
            {
                std::function<void(Payload&)> on_recv;
                std::function<void(Payload&)> on_send;

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

                bool bind(Port x)
                {
                    return sckt.bind(x) == sf::Socket::Done;
                }

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

        // Class that represents a packet buffer and a thread safe queue.
        // Can be used to receive and send queued packets.
        template <typename TTunnel>
        class ManagedPcktBuf
        {
        private:
            TTunnel& tunnel_;
            PayloadTSQueue tsq;

        protected:
            auto receive_payload(Payload& p)
            {
                return tunnel_.receive_payload(p);
            }

            auto send_payload(Payload& p) { return tunnel_.send_payload(p); }

        public:
            ManagedPcktBuf(TTunnel& t) : tunnel_{t} {}

            template <typename TDuration, typename... TArgs>
            bool try_enqueue_for(const TDuration& mDuration, TArgs&&... mArgs)
            {
                return tsq.try_enqueue_for(mDuration, FWD(mArgs)...);
            }

            template <typename TDuration>
            bool try_dequeue_for(const TDuration& mDuration, Payload& mOut)
            {
                return tsq.try_dequeue_for(mDuration, mOut);
            }

            auto& tunnel() noexcept { return tunnel_; }
        };
    }
}
