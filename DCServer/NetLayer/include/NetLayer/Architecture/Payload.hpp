#pragma once

#include "../Common/Common.hpp"
#include "../Architecture/ThreadSafeQueue.hpp"
#include "../Utils/Serialization.hpp"

namespace nl
{
    namespace Impl
    {
        struct PayloadTarget
        {
            IpAddr ip;
            Port port;

            PayloadTarget() = default;
            PayloadTarget(const IpAddr& mIp, Port mPort) noexcept : ip{mIp},
                                                                    port{mPort}
            {
            }
        };

        // Combination of data, ip and port.
        struct Payload
        {
            PcktBuf data;
            PayloadTarget target;

            Payload() = default;

            template <typename TData>
            Payload(TData&& mData, const PayloadTarget& mTarget)
                : data(FWD(mData)), target{mTarget}
            {
            }

            template <typename TData>
            Payload(TData&& mData, const IpAddr& mIp, Port mPort)
                : data(FWD(mData)), target{mIp, mPort}
            {
            }
        };

        using PayloadTSQueue = ThreadSafeQueue<Payload>;

        template <typename T>
        inline auto scktRecv(T& mSckt, Payload& mBuffer) noexcept
        {
            return mSckt.receive(
                mBuffer.data, mBuffer.target.ip, mBuffer.target.port);
        }

        template <typename T>
        inline auto scktSend(T& mSckt, Payload& mBuffer) noexcept
        {
            return mSckt.send(
                mBuffer.data, mBuffer.target.ip, mBuffer.target.port);
        }

        inline auto& operator<<(std::ostream& mS, const PayloadTarget& mX)
        {
            mS << mX.ip << ":" << mX.port;
            return mS;
        }

        inline auto& operator<<(std::ostream& mS, const Payload& mX)
        {
            mS << mX.target;
            return mS;
        }

        template <typename... Ts>
        inline auto mkPayload(const PayloadTarget& mTarget, Ts&&... mXs)
        {
            Payload p;
            p.target = mTarget;

            ssvu::forArgs(
                [&p](auto&& x)
                {
                    serialize_nl(p.data, x);
                },
                FWD(mXs)...);

            return p;
        }
    }
}
