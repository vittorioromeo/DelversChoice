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
				bool send(ScktUdp& mSckt, PcktBuf& mData, const IpAddr& mDestIp, Port mDestPort)
				{
					if(mSckt.send(FWD(mData), mDestIp, mDestPort) != sf::Socket::Done)
					{
						NL_DEBUGLO() << "Error sending packet\n";
						return false;
					}

					NL_DEBUGLO()
						<< "Sent packet to:\n"
						<< "\t" << mDestIp << ":" << mDestPort << "\n";

					return true;
				} 

			public:
				void sendLoop(ScktUdp& mSckt)
				{
					auto toSend(tsq.dequeue());
					NL_DEBUGLO() << "Dequeued packet\n";

					if(send(mSckt, toSend.data, toSend.ip, toSend.port))
					{

					}
				}
		};
	}
}