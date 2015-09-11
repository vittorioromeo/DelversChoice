#pragma once

#include "../Common/Common.hpp"
#include "../Architecture/ThreadSafeQueue.hpp"

namespace nl
{
	namespace Impl
	{
		// Combination of data, ip and port.
		struct Payload
		{
			PcktBuf data;
			IpAddr ip;
			Port port;

			Payload() = default;

			template<typename TData>
			Payload(TData&& mData, const IpAddr& mIp, Port mPort)
				: data(FWD(mData)), ip{mIp}, port{mPort}
			{

			}
		};

		using PayloadTSQueue = ThreadSafeQueue<Payload>;
	}
}