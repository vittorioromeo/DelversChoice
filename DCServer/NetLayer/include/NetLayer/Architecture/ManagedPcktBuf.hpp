#pragma once

#include "../Common/Common.hpp"
#include "../Architecture/ThreadSafeQueue.hpp"
#include "../Architecture/Payload.hpp"

namespace nl
{
    namespace Impl
    {
        // Class that represents a packet buffer and a thread safe queue.
        // Can be used to receive and send queued packets.
        class ManagedPcktBuf
        {
        private:
            PayloadTSQueue tsq;

        public:
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
        };
    }
}
