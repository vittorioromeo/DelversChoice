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
        impl::ManagedSendBuf<TTunnel> _mpb_send{_tunnel};

        // Internet -> recv queue/buf -> local host
        impl::ManagedRecvBuf<TTunnel> _mpb_recv{_tunnel};

        // TODO:
        // Threads:
        std::vector<std::unique_ptr<impl::busy_loop>> _busy_loops;
        mutable std::mutex _busy_loops_mutex;

        auto& get_busy_loops() { return _busy_loops; }
        const auto& get_busy_loops() const { return _busy_loops; }

        void emplace_default_busy_loops()
        {
            emplace_busy_loop([this]
                {
                    _mpb_send.send_step();
                });

            emplace_busy_loop([this]
                {
                    _mpb_recv.recv_step();
                });
        }

    public:
        template <typename... Ts>
        bool try_bind_tunnel(Ts&&... xs)
        {
            bool retry_res = retry(5, [&]
                {
                    return _tunnel.bind(FWD(xs)...);
                });

            if(retry_res)
            {
                nl::debugLo() << "Tunnel successfully bound.\n";

                emplace_default_busy_loops();
                return true;
            }

            std::cout << "Could not bind socket/tunnel.\n";
            std::terminate();
            return false;
        }

        auto bound() const noexcept { return _tunnel.bound(); }

        template <typename... TTunnelArgs>
        ManagedHostImpl(Port port, TTunnelArgs&&... ts)
            : _ip{IpAddr::getLocalAddress()}, _port{port}, _tunnel{FWD(ts)...}
        {
            get_busy_loops().reserve(100);
        }

        ~ManagedHostImpl() { stop(); }

        template <typename TF>
        auto& emplace_busy_loop(TF&& f)
        {
            auto l(make_unique_lock(_busy_loops_mutex));
            auto& bl_vec(get_busy_loops());

            bl_vec.emplace_back(std::make_unique<impl::busy_loop>(FWD(f)));
            return bl_vec.back();
        }

        bool busy() const noexcept
        {
            auto l(make_unique_lock(_busy_loops_mutex));
            auto& bl_vec(get_busy_loops());

            for(const auto& bf : bl_vec)
            {
                if(bf->busy())
                {
                    return true;
                }
            }

            return false;
        }

        void stop()
        {
            auto l(make_unique_lock(_busy_loops_mutex));
            auto& bl_vec(get_busy_loops());

            for(auto& bf : bl_vec)
            {
                bf->stop();
            }

            bl_vec.clear();
        }

        template <typename TDuration>
        auto try_send_payload_for(Payload& p, const TDuration& d)
        {
            return _mpb_send.try_enqueue_for(d, p);
        }

        auto try_send_payload(Payload& p)
        {
            return try_send_payload_for(p, 100ms);
        }

        template <typename TDuration, typename... Ts>
        auto try_make_and_send_payload_for(
            const PAddress& pa, const TDuration& d, Ts&&... xs)
        {
            auto p(make_payload(pa, FWD(xs)...));
            return try_send_payload_for(p, d);
        }

        template <typename... Ts>
        auto try_make_and_send_payload(const PAddress& pa, Ts&&... xs)
        {
            return try_make_and_send_payload_for(pa, 100ms, FWD(xs)...);
        }

        template <typename TF, typename TDuration>
        bool try_process_for(const TDuration& d, TF&& f)
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
            return try_process_for(100ms, f);
        }
    };

    using ManagedHost = ManagedHostImpl<Tunnel::UDPSckt>;
}
