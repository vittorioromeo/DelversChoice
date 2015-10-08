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
        template<typename TTunnel>
        class ManagedSendBuf : public ManagedPcktBuf<TTunnel>
        {
        private:
            using BaseType = ManagedPcktBuf<TTunnel>;

            Payload p;

            template <typename TFSend>
            bool send_impl(TFSend&& mFnSend)
            {
                // Try sending the next packet.
                if(!mFnSend())
                {
                    return false;
                }

                return true;
            }

            // Blocking function that sends enqueued packets.
            bool try_send_retry(std::size_t mTries)
            {
                return send_impl([this, mTries]
                    {
                        return retry(mTries, [this]
                            {
                                return this->send_payload(p);
                            });
                    });
            }

        public:
            using BaseType::BaseType;

            ~ManagedSendBuf() { NL_DEBUGLO() << "~sendbuf"; }

            auto sendLoop()
            {
                auto toSend(this->try_dequeue_for(100ms, p));

                if(toSend)
                {
                    return try_send_retry(5);
                }

                return false;
            }
        };
    }
}
