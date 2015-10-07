#pragma once

#include "../Common/Common.hpp"
#include "../Utils/Retry.hpp"
#include "../Architecture/ThreadSafeQueue.hpp"
#include "../Architecture/Payload.hpp"
#include "../Architecture/ManagedPcktBuf.hpp"

namespace nl
{
    namespace Impl
    {
        class ManagedRecvBuf : public ManagedPcktBuf
        {
        private:
            // Buffer for the received payload.
            Payload bp;

            bool recv_sckt_impl(ScktUdp& mSckt)
            {
                return scktRecv(mSckt, bp) == sf::Socket::Done;
            }

            template <typename TFRecv, typename TFEnqueue>
            bool recv_impl(
                ScktUdp& mSckt, TFRecv&& mFnRecv, TFEnqueue&& mFnEnqueue)
            {
                // Try receiving the next packet.
                if(!mFnRecv(mSckt))
                {
                    return false;
                }

                // If the packet was received, enqueue it.
                if(!mFnEnqueue(bp))
                {
                    return false;
                }

                return true;
            }

            // Blocking function that enqueues received packets.
            template <typename TDuration>
            bool try_recv_retry_for(
                ScktUdp& mSckt, std::size_t mTries, const TDuration& mDuration)
            {
                return recv_impl(mSckt,
                    [this, mTries](auto& s)
                    {
                        return retry(mTries, [this, &s]
                            {
                                return recv_sckt_impl(s);
                            });
                    },
                    [this, mDuration](auto& p)
                    {
                        return try_enqueue_for(mDuration, p);
                    });
            }

        public:
            // TODO: remove
            ~ManagedRecvBuf() { NL_DEBUGLO() << "~recvbuf"; }

            auto recvLoop(ScktUdp& mSckt)
            {
                // TODO: cv wait?

                return try_recv_retry_for(mSckt, 5, 100ms);
            }
        };
    }
}
