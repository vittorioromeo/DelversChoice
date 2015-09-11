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
		class BusyFlag
		{
			private:
				bool busy;

			public:
				BusyFlag(bool mX) noexcept : busy{mX} { }
				
				~BusyFlag() 
				{
					NL_DEBUGLO() << "busy=false\n";
					busy = false; 
				}

				void stop() noexcept { busy = false; }
				auto isBusy() const noexcept { return busy; }
		};

		template<typename... Ts>
		class BoundedFutures
		{
			private:
				std::tuple<std::future<Ts>...> futs;
				BusyFlag busy;

			public:
				template<typename... TArgs>
				BoundedFutures(TArgs&&... mArgs) :
					futs{std::async(std::launch::async, 
						[this, &mArgs]{ while(busy.isBusy()) FWD(mArgs)(); })...},
					busy{true} { }

				~BoundedFutures()
				{
					NL_DEBUGLO() << "~BoundedFutures\n";
				}

				void stop() noexcept { busy.stop(); }
				auto isBusy() const noexcept { return busy.isBusy(); }
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

			// Threads:
			// Local host -> send queue/buf -> internet
			// Internet -> recv queue/buf -> local host
			Impl::ManagedSendBuf mpbSend;
			Impl::ManagedRecvBuf mpbRecv;
			Impl::BoundedFutures<void, void> futs
			{
				[this]{ mpbSend.sendLoop(sckt); }, 
				[this]{ mpbRecv.recvLoop(sckt); }
			};

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
				sckt.setBlocking(true);
				tryBindSocket();
			}

			~ManagedHost()
			{
				sckt.unbind();
				NL_DEBUGLO() << "Destroyed ManagedHost\n";
			}

			auto isBusy() const noexcept
			{	
				return futs.isBusy();
			}

			void stop()
			{
				futs.stop();
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