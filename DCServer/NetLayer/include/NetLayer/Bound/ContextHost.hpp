#pragma once

#include <array>
#include "../Common/Common.hpp"
#include "../Architecture/Architecture.hpp"
#include "/home/vittorioromeo/OHWorkspace/cppcon2015/Other/Other.hpp"
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
        using Payload = nl::Impl::Payload;
        using PayloadAddress = nl::Impl::PayloadAddress;

        MyDispatchTable dispatchTable;
        nl::ManagedHost managedHost;

        template<typename T>
        void send_with_header(const PayloadAddress& pa, T x)
        {
            constexpr auto id(TConfig::template getPcktBindID<T>());
            // ssvu::lo("SENT ID") << id << "\n";
            managedHost.send(pa, id, x);
        }

    public:
        ContextHost(nl::Port port) : managedHost(port)
        {
            auto fnProcess([this](nl::PcktBuf& data, const auto& sender)
                {
                    dispatchTable.process(sender, data);
                });

            managedHost.emplaceBusyFut([this, fnProcess]
                {
                    managedHost.try_process(fnProcess);
                });
        }

        template <typename T>
        void send(const PayloadAddress& pa, T x)
        {
            send_with_header<T>(pa, x);
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
