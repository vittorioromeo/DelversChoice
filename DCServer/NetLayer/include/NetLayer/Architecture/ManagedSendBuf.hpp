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
        class ManagedSendBuf : public ManagedPcktBuf
        {
        private:
            bool send_sckt_impl(ScktUdp& mSckt, Payload& mP)
            {
                return scktSend(mSckt, mP) == sf::Socket::Done;
            }

            template <typename TFSend>
            bool send_impl(ScktUdp& mSckt, Payload& mP, TFSend&& mFnSend)
            {
                // Try receiving the next packet.
                if(!mFnSend(mSckt, mP)) {
                    return false;
                }

                return true;
            }

            // Blocking function that sends enqueued packets.
            bool try_send_retry(ScktUdp& mSckt, Payload& mP, std::size_t mTries)
            {
                return send_impl(mSckt, mP, [this, mTries](auto& s, auto& p)
                {
                    return retry(mTries, [this, &s, &p]
                                 {
                                     return send_sckt_impl(s, p);
                                 });
                });
            }

        public:
            ~ManagedSendBuf() { NL_DEBUGLO() << "~sendbuf"; }

            auto sendLoop(ScktUdp& mSckt)
            {
                Payload p;

                // NL_DEBUGLO() << "try dequeue...\n";
                auto toSend(tsq.try_dequeue_for(100ms, p));

                if(toSend) {
                    // NL_DEBUGLO() << "try dequeue... OK\n";
                    return try_send_retry(mSckt, p, 5);
                }

                // NL_DEBUGLO() << "try dequeue... fail\n";
                return false;
            }
        };
    }
}
