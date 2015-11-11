#pragma once

#include <array>
#include "../Common/Common.hpp"
#include "../Architecture/Architecture.hpp"
//#include "/home/vittorioromeo/OHWorkspace/cppcon2015/Other/Other.hpp"
#include "../Serialization/Serialization.hpp"

// namespace MPL = ::ecs::MPL;

namespace experiment
{
    template <typename TConfig>
    class ContextHost
    {
    private:
        using Config = TConfig;
        using MyDispatchTable = DispatchTable<Config>;
        using Payload = nl::Payload;
        using PAddress = nl::PAddress;
        using Tunnel = typename Config::Tunnel;

        MyDispatchTable _dispatch_table;
        nl::ManagedHostImpl<Tunnel> _host;

        template <typename T, typename TDuration, typename TPckt>
        auto try_send_pckt_with_header_for(
            const PAddress& pa, const TDuration& d, TPckt&& pckt)
        {
            constexpr auto id(Config::template getPcktBindID<T>());
            return _host.try_make_and_send_payload_for(pa, d, id, FWD(pckt));
        }

        template <typename T, typename TPckt>
        auto try_send_pckt_with_header(const PAddress& pa, TPckt&& pckt)
        {
            return try_send_pckt_with_header_for<T>(pa, 100ms, FWD(pckt));
        }

        template <typename T>
        void fn_process(nl::PcktBuf& data, const T& sender)
        {
            _dispatch_table.process(sender, data);
        }


    public:
        ContextHost(nl::Port port) : _host(port) {}

        template <typename... Ts>
        auto try_bind_tunnel(Ts&&... xs)
        {
            return _host.try_bind_tunnel(FWD(xs)...);
        }

        auto bound() const noexcept { return _host.bound(); }

        void stop() { _host.stop(); }

        template <typename T, typename TDuration>
        auto try_send_pckt_for(const PAddress& pa, const TDuration& d, T&& pckt)
        {
            return try_send_pckt_with_header_for<T>(pa, d, FWD(pckt));
        }

        template <typename T>
        auto try_send_pckt(const PAddress& pa, T&& pckt)
        {
            return try_send_pckt_with_header<T>(pa, FWD(pckt));
        }

        template <typename T, typename TDuration, typename... Ts>
        auto try_make_and_send_pckt_for(
            const PAddress& pa, const TDuration& d, Ts&&... xs)
        {
            return try_send_pckt_for<T>(pa, d, nl::make_pckt<T>(FWD(xs)...));
        }

        template <typename T, typename... Ts>
        auto try_make_and_send_pckt(const PAddress& pa, Ts&&... xs)
        {
            return try_send_pckt<T>(pa, nl::make_pckt<T>(FWD(xs)...));
        }


        template <typename TPckt, typename TF>
        void on(TF&& fn)
        {
            _dispatch_table.template add<TPckt>(fn);
        }

        template <typename TPckt, typename TF>
        void on_d(TF&& fn)
        {
            _dispatch_table.template addDestructured<TPckt>(fn);
        }

        bool busy() const noexcept { return _host.busy(); }
        auto& host() noexcept { return _host; }

        template <typename TDuration>
        auto try_dispatch_and_process_for(const TDuration& d)
        {
            return _host.try_process_for(d, [this](auto&&... xs)
                {
                    this->fn_process(FWD(xs)...);
                });
        }

        auto try_dispatch_and_process()
        {
            return try_dispatch_and_process_for(100ms);
        }
    };
}
