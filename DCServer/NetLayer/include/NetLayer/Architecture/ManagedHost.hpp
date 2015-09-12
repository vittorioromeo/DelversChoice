#pragma once

#include "../Common/Common.hpp"
#include "../Architecture/ThreadSafeQueue.hpp"
#include "../Architecture/Payload.hpp"
#include "../Architecture/ManagedSendBuf.hpp"
#include "../Architecture/ManagedRecvBuf.hpp"

namespace nl
{
	namespace Impl
	{
		class BusyFut
		{
			private:
				std::atomic<bool> wrapperAlive{true}; // construct flag first!
				std::future<void> fut;

			public:
				std::string xd;
				template<typename TF>
				BusyFut(const TF& mFn) :
					fut{std::async(std::launch::async, [this, mFn]
					{
						while(wrapperAlive)
						{
							//NL_DEBUGLO() << xd << (wrapperAlive ? "alive" : "dead");
							mFn();


						}

						NL_DEBUGLO() << xd << (wrapperAlive ? "alive" : "dead!!");
					})}
				{

				}

				~BusyFut() 
				{
					wrapperAlive = false;
					NL_DEBUGLO() << xd << "beforeget";
					fut.get(); // block, so it sees wrapperAlive before it is destroyed.
					NL_DEBUGLO() << xd << "aftgerget";
				}

				void stop() { wrapperAlive = false; }
				bool isBusy() const { return wrapperAlive; }
			};
	}

	class ManagedHost
	{
		private:
			// This host's ip.
			IpAddr ip;

			// This host's port.
			Port port;

			// This host's socket.
			ScktUdp sckt;

			
			// Local host -> send queue/buf -> internet
			Impl::ManagedSendBuf mpbSend;
			
			// Internet -> recv queue/buf -> local host
			Impl::ManagedRecvBuf mpbRecv;

			// TODO:
			// Threads:
			Impl::BusyFut futSend{[this]{ mpbSend.sendLoop(sckt); }};
			Impl::BusyFut futRecv{[this]{ mpbRecv.recvLoop(sckt); }};
			
			void tryBindSocket()
			{
				futSend.xd = "send ";
				futRecv.xd = "recv ";

				if(sckt.bind(port) != sf::Socket::Done)
				{
					throw std::runtime_error("Error binding socket");
				}
				else
				{
					ssvu::lo() << "Socket successfully bound to port " + ssvu::toStr(port) + "\n";
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
				: ip{IpAddr::getLocalAddress()}, port{mPort}
			{
				sckt.setBlocking(false);
				tryBindSocket();
			}

			~ManagedHost()
			{
				sckt.unbind();
				stop();
				NL_DEBUGLO() << "Destroyed ManagedHost\n";
			}

			bool isBusy() const noexcept
			{
				return futSend.isBusy() || futRecv.isBusy();
			}

			void stop()
			{
				futSend.stop();
				futRecv.stop();
			}

			// TODO: use payload
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