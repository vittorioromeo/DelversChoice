#pragma once

#include "../Common/Common.hpp"
#include "../Utils/Retry.hpp"
#include "../Architecture/ThreadSafeQueue.hpp"
#include "../Payload/Payload.hpp"
#include "../Architecture/ManagedPcktBuf.hpp"

namespace nl
{
    namespace impl
    {
        template <typename TTunnel>
        class ManagedRecvBuf : public ManagedPcktBuf<TTunnel>
        {
        private:
            using BaseType = ManagedPcktBuf<TTunnel>;

            // Buffer for the received payload.
            Payload p;

            template <typename TFRecv, typename TFEnqueue>
            bool recv_impl(TFRecv&& f_recv, TFEnqueue&& f_enqueue)
            {
                // Try receiving the next packet.
                if(!f_recv())
                {
                    return false;
                }

                // If the packet was received, enqueue it.
                if(!f_enqueue())
                {
                    return false;
                }

                return true;
            }

            // Blocking function that enqueues received packets.
            template <typename TDuration>
            bool try_recv_retry_for(
                std::size_t tries, const TDuration& duration)
            {
                return recv_impl(
                    [this, tries]
                    {
                        return retry(tries, [this]
                            {
                                return this->receive_payload(p);
                            });
                    },
                    [this, duration]
                    {
                        return this->try_enqueue_for(duration, p);
                    });
            }

        public:
            using BaseType::BaseType;

            auto recv_step()
            {
                // TODO: cv wait?
                return try_recv_retry_for(5, 100ms);
            }
        };
    }
}
