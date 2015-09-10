#pragma once

#include "../Common/Common.hpp"
#include "../Architecture/ThreadSafeQueue.hpp"
#include "../Architecture/Payload.hpp"
#include "../Architecture/ManagedSendBuf.hpp"
#include "../Architecture/ManagedRecvBuf.hpp"

namespace nl
{	
	class ManagedHost
	{
		private:
			// This host's ip.
			IpAddr ip;

			// This host's port.
			Port port;

			// This host's socket.
			ScktUdp sckt;

			// Is this host busy?
			bool busy;

			// Local host -> send queue/buf -> internet
			Impl::ManagedSendBuf mpbSend; 
			std::future<void> futSend;

			// Internet -> recv queue/buf -> local host
			Impl::ManagedRecvBuf mpbRecv;
			std::future<void> futRecv;

			void tryBindSocket()
			{
				if(sckt.bind(port) != sf::Socket::Done)
				{
					throw std::runtime_error("Error binding socket");
				}
				else
				{
					ssvu::lo() << "Socket successfully bound to port " + ssvu::toStr(port) + "\n";
				}
			}

			void sendThread()
			{
				while(busy)
				{
					mpbSend.sendLoop(sckt);
				}
			}

			void recvThread()
			{
				while(busy)
				{
					mpbRecv.recvLoop(sckt);
				}
			}

			template<typename TF>
			bool processImpl(TF&& mFn)
			{
				if(mpbRecv.empty()) return false;
				
				auto payload(mpbRecv.dequeue());
				mFn(payload.data, payload.ip, payload.port);
				return true;
			}

		public:
			ManagedHost(Port mPort)
				: ip{IpAddr::getLocalAddress()}, port{mPort}, busy{true},
				futSend{std::async(std::launch::async, [this]{ sendThread(); })},
				futRecv{std::async(std::launch::async, [this]{ recvThread(); })}
			{
				sckt.setBlocking(true);
				tryBindSocket();
			}

			~ManagedHost()
			{
				busy = false;
			}

			auto isBusy() const noexcept
			{
				return busy;
			}

			void stop()
			{
				busy = false;
			}

			template<typename TData>
			void send(TData&& mData, const IpAddr& mDestIp, Port mDestPort)
			{
				mpbSend.enqueue(FWD(mData), mDestIp, mDestPort);
			}

			template<typename TF>
			void process(TF&& mFn)
			{
				while(processImpl(mFn)) { }
			}
	};
}