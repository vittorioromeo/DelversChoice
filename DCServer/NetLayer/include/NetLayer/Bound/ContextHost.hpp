#pragma once

#include <array>
#include "../Common/Common.hpp"
#include "../Architecture/Architecture.hpp"
//#include "/home/vittorioromeo/OHWorkspace/cppcon2015/Other/Other.hpp"
#include "../Serialization/Serialization.hpp"

namespace MPL = ::ecs::MPL;

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

        MyDispatchTable _dispatch_table;
        nl::ManagedHost _host;

        template <typename T, typename TX>
        void send_with_header(const PAddress& pa, TX&& x)
        {
            constexpr auto id(Config::template getPcktBindID<T>());
            _host.send(pa, id, FWD(x));
        }

        template <typename T>
        void fn_proecss(nl::PcktBuf& data, const T& sender)
        {
            _dispatch_table.process(sender, data);
        }


    public:
        ContextHost(nl::Port port) : _host(port) {}

        void stop() { _host.stop(); }

        template <typename T, typename TX>
        void send(const PAddress& pa, TX&& x)
        {
            send_with_header<T>(pa, FWD(x));
        }

        template <typename T, typename... Ts>
        void make_and_send(const PAddress& pa, Ts&&... xs)
        {
            send<T>(pa, nl::make_pckt<T>(FWD(xs)...));
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

        bool try_dispatch_and_process()
        {
            return _host.try_process([this](auto&&... xs)
                {
                    this->fn_proecss(FWD(xs)...);
                });
        }
    };
}
