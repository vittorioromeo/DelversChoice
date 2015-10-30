#pragma once

#include "../Common/Common.hpp"
#include "../Utils/Retry.hpp"
#include "../Architecture/BusyFut.hpp"
#include "../Architecture/ThreadSafeQueue.hpp"
#include "../Payload/Payload.hpp"
#include "../Architecture/ManagedPcktBuf.hpp"
#include "../Architecture/ManagedSendBuf.hpp"
#include "../Architecture/ManagedRecvBuf.hpp"

namespace nl
{
    // TODO: templatize with PayloadProvider/Sink ?
    template <typename TTunnel>
    class ManagedHostImpl
    {
    private:
        // This host's ip.
        IpAddr ip;

        // This host's port.
        Port port;

        // This host's socket.
        TTunnel tunnel;

        // Local host -> send queue/buf -> internet
        Impl::ManagedSendBuf<TTunnel> mpbSend{tunnel};

        // Internet -> recv queue/buf -> local host
        Impl::ManagedRecvBuf<TTunnel> mpbRecv{tunnel};

        // TODO:
        // Threads:
        std::vector<std::unique_ptr<Impl::BusyFut>> busyFutures;

        bool tryBindSocket()
        {
            if(retry(5, [this]
                   {
                       return tunnel.bind(port);
                   }))
            {
                ssvu::lo() << "Socket successfully bound to port " << port
                           << "\n";

                return true;
            }

            return false;
        }

    public:
        ManagedHostImpl(Port mPort) : ip{IpAddr::getLocalAddress()}, port{mPort}
        {
            // busyFutures.reserve(100);

            // TODO: static_if TTunnel...
            tryBindSocket();

            emplaceBusyFut([this]
                {
                    mpbSend.sendLoop();
                });

            emplaceBusyFut([this]
                {
                    mpbRecv.recvLoop();
                });
        }

        ~ManagedHostImpl()
        {
            stop();
            ::nl::debugLo() << "Destroyed ManagedHost\n";
        }

        template <typename TF>
        void emplaceBusyFut(TF&& fn)
        {
            busyFutures.emplace_back(std::make_unique<Impl::BusyFut>(FWD(fn)));
        }

        bool isBusy() const noexcept
        {
            for(const auto& bf : busyFutures)
                if(bf->isBusy()) return true;

            return false;
        }

        void stop()
        {
            for(auto& bf : busyFutures) bf->stop();
        }

        bool send(Impl::Payload& mPayload)
        {
            // TODO:
            return mpbSend.try_enqueue_for(100ms, mPayload);
        }

        template <typename... Ts>
        bool send(const Impl::PayloadAddress& mTarget, Ts&&... mXs)
        {
            auto p(Impl::make_payload(mTarget, FWD(mXs)...));
            return send(p);
        }

        template <typename TF>
        bool try_process(TF&& f)
        {
            Impl::Payload p;

            // TODO:
            auto ok(mpbRecv.try_dequeue_for(100ms, p));

            if(ok)
            {
                f(p.data, p.address);
                return true;
            }

            return false;
        }
    };

    using ManagedHost = ManagedHostImpl<Impl::Tunnel::UDPSckt>;
}
