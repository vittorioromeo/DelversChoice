#pragma once

#include "../Common/Common.hpp"
#include "../Architecture/ThreadSafeQueue.hpp"
#include "../Architecture/Payload.hpp"
#include "../Architecture/ManagedPcktBuf.hpp"

namespace nl
{
    namespace Impl
    {
        class ManagedRecvBuf : public ManagedPcktBuf
        {
        private:
            // Buffer for the received payload.
            Payload bp;

            // Blocking function that enqueues received packets.
            bool recv(ScktUdp& mSckt)
            {
                // Try receiving the next packet.
                if(scktRecv(mSckt, bp) != sf::Socket::Done) {
                    // NL_DEBUGLO() << "Error receiving packet\n";
                    return false;
                }

                // If the packet was received, enqueue it.
                tsq.enqueue(bp);

                NL_DEBUGLO() << "Received packet from:\n"
                             << "\t" << bp << "\n";

                return true;
            }

        public:
            // TODO: remove
            ~ManagedRecvBuf() { NL_DEBUGLO() << "~recvbuf"; }

            auto recvLoop(ScktUdp& mSckt)
            {
                // TODO: cv wait?

                // NL_DEBUGLO() << "Clearing recv buffer\n";
                // bp.data.clear();
                // NL_DEBUGLO() << "Cleared recv buffer\n";

                return recv(mSckt);
            }
        };
    }
}
