#pragma once

#include "../Common/Common.hpp"
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
            // Blocking function that sends enqueued packets.
            bool send(ScktUdp& mSckt, Payload& mP)
            {
                if(retry(7, [this, &mSckt, &mP]
                         {
                             return scktSend(mSckt, mP) != sf::Socket::Done;
                         }))
                {
                    NL_DEBUGLO() << "Error sending packet\n";

                    return false;
                }


                NL_DEBUGLO() << "Sent packet to:\n"
                             << "\t" << mP << "\n";

                return true;
            }

        public:
            ~ManagedSendBuf() { NL_DEBUGLO() << "~sendbuf"; }

            auto sendLoop(ScktUdp& mSckt)
            {
                // TODO: cv wait?
                // if(tsq.empty()) return false;

                auto toSend(tsq.dequeue());
                NL_DEBUGLO() << "Dequeued packet\n";

                return send(mSckt, toSend);
            }
        };
    }
}
