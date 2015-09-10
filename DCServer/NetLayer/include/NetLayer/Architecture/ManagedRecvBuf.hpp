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
				PcktBuf buf;
				IpAddr senderIp;
				Port senderPort;

				// Blocking function that enqueues received packets.
				bool recv(ScktUdp& mSckt)
				{
					// Try receiving the next packet.
					if(mSckt.receive(buf, senderIp, senderPort) != sf::Socket::Done)
					{
						NL_DEBUGLO() << "Error receiving packet\n";
						return false;
					}
				
					// If the packet was received, enqueue it.
					tsq.enqueue(std::move(buf), senderIp, senderPort);

					NL_DEBUGLO()
						<< "Received packet from:\n"
						<< "\t" << senderIp << ":" << senderPort << "\n";
					
					return true;
				}

			public:
				void recvLoop(ScktUdp& mSckt)
				{
					buf.clear();
					NL_DEBUGLO() << "Clearing recv buffer\n";

					if(recv(mSckt))
					{

					}
				}
		};
	}
}