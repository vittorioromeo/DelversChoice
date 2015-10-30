#pragma once

#include "../Common/Common.hpp"
#include "../Architecture/ThreadSafeQueue.hpp"
#include "../Payload/Payload.hpp"
#include "../Tunnel/Tunnel.hpp"

namespace nl
{
    namespace Impl
    {
        using PayloadTSQueue = ThreadSafeQueue<Payload>;
    }

    namespace Impl
    {
        // Class that represents a packet buffer and a thread safe queue.
        // Can be used to receive and send queued packets.
        template <typename TTunnel>
        class ManagedPcktBuf
        {
        private:
            TTunnel& _tunnel;
            PayloadTSQueue _tsq;

        protected:
            auto receive_payload(Payload& p)
            {
                return _tunnel.receive_payload(p);
            }

            auto send_payload(Payload& p) { return _tunnel.send_payload(p); }

        public:
            ManagedPcktBuf(TTunnel& t) : _tunnel{t} {}

            template <typename TDuration, typename... TArgs>
            bool try_enqueue_for(const TDuration& mDuration, TArgs&&... mArgs)
            {
                return _tsq.try_enqueue_for(mDuration, FWD(mArgs)...);
            }

            template <typename TDuration>
            bool try_dequeue_for(const TDuration& mDuration, Payload& mOut)
            {
                return _tsq.try_dequeue_for(mDuration, mOut);
            }

            auto& tunnel() noexcept { return _tunnel; }
        };
    }
}
