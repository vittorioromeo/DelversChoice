#pragma once

#include "../Common/Common.hpp"
#include "../Utils/Retry.hpp"
#include "../Architecture/BusyFut.hpp"
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

        // Local host -> send queue/buf -> internet
        Impl::ManagedSendBuf mpbSend;

        // Internet -> recv queue/buf -> local host
        Impl::ManagedRecvBuf mpbRecv;

        // TODO:
        // Threads:
        std::vector<std::unique_ptr<Impl::BusyFut>> busyFutures;

        void tryBindSocket()
        {
            if(retry(5, [this]
                   {
                       return sckt.bind(port) == sf::Socket::Done;
                   }))
            {
                ssvu::lo() << "Socket successfully bound to port " << port
                           << "\n";
            }
            else
            {
                throw std::runtime_error("Error binding socket");
            }
        }

    public:
        ManagedHost(Port mPort) : ip{IpAddr::getLocalAddress()}, port{mPort}
        {
            // busyFutures.reserve(100);

            sckt.setBlocking(false);
            tryBindSocket();

            emplaceBusyFut([this]
                {
                    mpbSend.sendLoop(sckt);
                });

            emplaceBusyFut([this]
                {
                    mpbRecv.recvLoop(sckt);
                });
        }

        ~ManagedHost()
        {
            sckt.unbind();
            stop();
            NL_DEBUGLO() << "Destroyed ManagedHost\n";
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

        void send(Impl::Payload& mPayload)
        {
            // TODO:
            mpbSend.tsq.try_enqueue_for(100ms, mPayload);
        }

        template <typename... Ts>
        void send(const Impl::PayloadTarget& mTarget, Ts&&... mXs)
        {
            auto p(Impl::mkPayload(mTarget, FWD(mXs)...));
            send(p);
        }

        template <typename TF>
        void try_process(TF&& f)
        {
            Impl::Payload p;

            // TODO:
            auto ok(mpbRecv.tsq.try_dequeue_for(100ms, p));

            if(ok)
            {
                f(p.data, p.target);
            }
        }
    };
}
