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

        MyDispatchTable dispatchTable;
        nl::ManagedHost managedHost;

        template <typename T, typename TX>
        void send_with_header(const PAddress& pa, TX&& x)
        {
            constexpr auto id(Config::template getPcktBindID<T>());
            managedHost.send(pa, id, FWD(x));
        }

    public:
        ContextHost(nl::Port port) : managedHost(port)
        {
            auto fnProcess([this](nl::PcktBuf& data, const auto& sender)
                {
                    dispatchTable.process(sender, data);
                });

            managedHost.emplace_busy_loop([this, fnProcess]
                {
                    managedHost.try_process(fnProcess);
                });
        }

        void stop()
        {
            managedHost.stop();
        }

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
            dispatchTable.template add<TPckt>(fn);
        }

        template <typename TPckt, typename TF>
        void on_d(TF&& fn)
        {
            dispatchTable.template addDestructured<TPckt>(fn);
        }

        bool busy() const noexcept { return managedHost.isBusy(); }
        auto& host() noexcept { return managedHost; }
    };
}
