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
        protected:
            PayloadTSQueue tsq;

        public:
            template <typename... TArgs>
            void enqueue(TArgs&&... mArgs)
            {
                tsq.enqueue(FWD(mArgs)...);
            }

            auto dequeue() { return tsq.dequeue(); }

            auto empty() const { return tsq.empty(); }
        };
    }
}
