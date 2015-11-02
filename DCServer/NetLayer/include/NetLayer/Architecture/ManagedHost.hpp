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
    template <typename TTunnel>
    class ManagedHostImpl
    {
    private:
        // This host's ip.
        IpAddr _ip;

        // This host's port.
        Port _port;

        // This host's socket.
        TTunnel _tunnel;

        // Local host -> send queue/buf -> internet
        Impl::ManagedSendBuf<TTunnel> _mpb_send{_tunnel};

        // Internet -> recv queue/buf -> local host
        Impl::ManagedRecvBuf<TTunnel> _mpb_recv{_tunnel};

        // TODO:
        // Threads:
        std::vector<std::unique_ptr<Impl::busy_loop>> _busy_loops;

        bool try_bind_tunnel()
        {
            if(retry(5, [this]
                   {
                       return _tunnel.bind(_port);
                   }))
            {
                ssvu::lo() << "Socket successfully bound to port " << _port
                           << "\n";

                return true;
            }

            return false;
        }

    public:
        template <typename... TTunnelArgs>
        ManagedHostImpl(Port port, TTunnelArgs&&... ts)
            : _ip{IpAddr::getLocalAddress()}, _port{port}, _tunnel{FWD(ts)...}
        {
            if(!try_bind_tunnel())
            {
                std::cout << "Could not bind socket/tunnel.\n";
                std::terminate();
            }

            emplace_busy_loop([this]
                {
                    _mpb_send.send_step();
                });

            emplace_busy_loop([this]
                {
                    _mpb_recv.recv_step();
                });
        }

        ~ManagedHostImpl()
        {
            stop();
            ::nl::debugLo() << "Destroyed ManagedHost\n";
        }

        template <typename TF>
        auto& emplace_busy_loop(TF&& f)
        {
            _busy_loops.emplace_back(std::make_unique<Impl::busy_loop>(FWD(f)));

            return _busy_loops.back();
        }

        bool busy() const noexcept
        {
            for(const auto& bf : _busy_loops)
                if(bf->busy()) return true;

            return false;
        }

        void stop()
        {
            for(auto& bf : _busy_loops) bf->stop();
        }

        bool send(Payload& p)
        {
            // TODO:
            return _mpb_send.try_enqueue_for(100ms, p);
        }

        template <typename... Ts>
        bool send(const PAddress& pa, Ts&&... mXs)
        {
            auto p(make_payload(pa, FWD(mXs)...));
            return send(p);
        }

        template <typename TF, typename TDuration>
        bool try_process_for(TF&& f, const TDuration& d)
        {
            Payload p;

            // TODO:
            auto ok(_mpb_recv.try_dequeue_for(d, p));

            if(!ok) return false;

            f(p.data, p.address);
            return true;
        }

        template <typename TF>
        bool try_process(TF&& f)
        {
            return try_process_for(f, 100ms);
        }
    };

    using ManagedHost = ManagedHostImpl<Impl::Tunnel::UDPSckt>;
}
