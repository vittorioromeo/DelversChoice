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
        template <typename TTunnel>
        class ManagedRecvBuf : public ManagedPcktBuf<TTunnel>
        {
        private:
            using BaseType = ManagedPcktBuf<TTunnel>;

            // Buffer for the received payload.
            Payload p;

            template <typename TFRecv, typename TFEnqueue>
            bool recv_impl(TFRecv&& mFnRecv, TFEnqueue&& mFnEnqueue)
            {
                // Try receiving the next packet.
                if(!mFnRecv())
                {
                    return false;
                }

                // If the packet was received, enqueue it.
                if(!mFnEnqueue())
                {
                    return false;
                }

                return true;
            }

            // Blocking function that enqueues received packets.
            template <typename TDuration>
            bool try_recv_retry_for(
                std::size_t mTries, const TDuration& mDuration)
            {
                return recv_impl(
                    [this, mTries]
                    {
                        return retry(mTries, [this]
                            {
                                return this->receive_payload(p);
                            });
                    },
                    [this, mDuration]
                    {
                        return this->try_enqueue_for(mDuration, p);
                    });
            }

        public:
            using BaseType::BaseType;

            // TODO: remove
            ~ManagedRecvBuf() { NL_DEBUGLO() << "~recvbuf"; }

            auto recvLoop()
            {
                // TODO: cv wait?
                return try_recv_retry_for(5, 100ms);
            }
        };
    }
}
