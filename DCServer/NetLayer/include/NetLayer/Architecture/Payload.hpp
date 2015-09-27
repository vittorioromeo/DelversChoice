#pragma once

#include "../Common/Common.hpp"
#include "../Architecture/ThreadSafeQueue.hpp"

namespace nl
{
    namespace Impl
    {
        struct PayloadTarget
        {
            IpAddr ip;
            Port port;

            PayloadTarget() = default;

            PayloadTarget(const IpAddr& mIp, Port mPort) : ip{mIp}, port{mPort}
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
            return mSckt.receive(mBuffer.data, mBuffer.target.ip,
                                 mBuffer.target.port);
        }

        template <typename T>
        inline auto scktSend(T& mSckt, Payload& mBuffer) noexcept
        {
            return mSckt.send(mBuffer.data, mBuffer.target.ip,
                              mBuffer.target.port);
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
        inline auto makePayload(const PayloadTarget& mTarget, Ts&&... mXs)
        {
            Payload p;
            p.target = mTarget;

            ssvu::forArgs(
                [&p](auto&& x)
                {
                    p.data << FWD(x);
                },
                FWD(mXs)...);

            return p;
        }
    }

    // TODO: move
    template <typename TF>
    bool retry(std::size_t mTimes, TF&& mFn)
    {
        for(auto i(0u); i < mTimes; ++i) {
            if(mFn()) {
                return true;
            }

            std::this_thread::sleep_for(100ms * i);

            if(i > 4) {
                NL_DEBUGLO() << i << "th retry...\n";
            }
        }

        NL_DEBUGLO() << "failed!...\n";
        return false;
    }
}