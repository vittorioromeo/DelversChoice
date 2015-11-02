#pragma once

#include "../Common/Common.hpp"
#include "../Utils/Retry.hpp"
#include "../Architecture/ThreadSafeQueue.hpp"
#include "../Payload/Payload.hpp"
#include "../Architecture/ManagedPcktBuf.hpp"

namespace nl
{
    namespace Impl
    {
        template <typename TTunnel>
        class ManagedSendBuf : public ManagedPcktBuf<TTunnel>
        {
        private:
            using BaseType = ManagedPcktBuf<TTunnel>;

            Payload p;

            template <typename TFSend>
            bool send_impl(TFSend&& f_send)
            {
                // Try sending the next packet.
                if(!f_send())
                {
                    return false;
                }

                return true;
            }

            // Blocking function that sends enqueued packets.
            bool try_send_retry(std::size_t tries)
            {
                return send_impl([this, tries]
                    {
                        return retry(tries, [this]
                            {
                                return this->send_payload(p);
                            });
                    });
            }

        public:
            using BaseType::BaseType;

            ~ManagedSendBuf() { ::nl::debugLo() << "~sendbuf"; }

            auto send_step()
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
