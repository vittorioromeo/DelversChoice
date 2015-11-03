#pragma once

#include "../Common/Common.hpp"
#include "../Architecture/ThreadSafeQueue.hpp"
#include "../Payload/Payload.hpp"

namespace nl
{
    namespace impl
    {
        using payload_ts_queue = ThreadSafeQueue<Payload>;
    }

    namespace impl
    {
        // Class that represents a packet buffer and a thread safe queue.
        // Can be used to receive and send queued packets.
        template <typename TTunnel>
        class ManagedPcktBuf
        {
        private:
            TTunnel& _tunnel;
            payload_ts_queue _tsq;

        protected:
            auto receive_payload(Payload& p)
            {
                return _tunnel.receive_payload(p);
            }

            auto send_payload(Payload& p) { return _tunnel.send_payload(p); }

        public:
            ManagedPcktBuf(TTunnel& t) : _tunnel{t} {}

            template <typename TDuration, typename... Ts>
            bool try_enqueue_for(const TDuration& d, Ts&&... xs)
            {
                return _tsq.try_enqueue_for(d, FWD(xs)...);
            }

            template <typename TDuration>
            bool try_dequeue_for(const TDuration& d, Payload& p)
            {
                return _tsq.try_dequeue_for(d, p);
            }

            auto& tunnel() noexcept { return _tunnel; }
        };
    }
}
