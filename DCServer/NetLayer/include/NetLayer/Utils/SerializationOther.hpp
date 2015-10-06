#pragma once

#include <array>
#include "../Common/Common.hpp"
#include "../Architecture/Architecture.hpp"
#include "/home/vittorioromeo/OHWorkspace/cppcon2015/Other/Other.hpp"
#include "./Serialization.hpp"

namespace MPL = ::ecs::MPL;

namespace experiment
{
    // Settings forward-declaration.
    template <typename>
    struct Settings;

    template <typename TIDType = nl::UInt32>
    struct Settings
    {
        using IDType = TIDType;
    };

    template <typename TType>
    struct PcktBind
    {
        using Type = TType;
    };

    template <typename T>
    using PcktBindType = typename T::Type;

    template <typename... Ts>
    using PcktBinds = MPL::TypeList<Ts...>;

    template <typename TList>
    using PcktBindsTypes = MPL::Map<PcktBindType, TList>;

    template <typename TSettings, typename TPcktBinds>
    struct Config
    {
        using IDType = typename TSettings::IDType;
        using Settings = TSettings;
        using PcktBinds = TPcktBinds;
        using BindsTypes = PcktBindsTypes<PcktBinds>;

        // static_assert validity of settings
        // static_assert validity of packet binds

        static constexpr std::size_t pcktBindsCount{MPL::size<PcktBinds>()};

        template <typename T>
        static constexpr auto hasPcktBindFor() noexcept
        {
            return MPL::Contains<T, BindsTypes>{};
        }

        template <typename T>
        static constexpr auto getPcktBindID() noexcept
        {
            return static_cast<IDType>(MPL::IndexOf<T, BindsTypes>{});
        }

        template <typename T>
        using PcktTypes = MPL::TypeList<int, float, char>; // TODO
    };


    template <typename TConfig>
    class DispatchTable
    {
    private:
        using IDType = typename TConfig::IDType;
        static constexpr auto pcktBindsCount = TConfig::pcktBindsCount;

        std::array<std::function<void(nl::Impl::PayloadTarget, nl::PcktBuf&)>,
            pcktBindsCount> fncs;

    public:
        template <typename T, typename TF>
        void add(TF&& fnToCall)
        {
            constexpr auto id(TConfig::template getPcktBindID<T>());

            auto& fn(fncs[id]);

            // Assume id has already been taken from
            fn = [id, fnToCall](const auto& pt, auto& buf)
            {
                T obj;
                buf >> obj;

                fnToCall(pt, obj);
            };
        }

        template <typename T, typename TF>
        void addDestructured(TF&& fnToCall)
        {
            add<T>([fnToCall](auto& pt, auto& o)
                {
                    auto boundfn = [fnToCall, pt](auto&&... xs)
                    {
                        return fnToCall(pt, FWD(xs)...);
                    };
                    ecs::Utils::tupleApply(boundfn, o.fields);
                });
        }

        void process(const nl::Impl::PayloadTarget& sender, nl::PcktBuf& buf)
        {
            // Assumes id is still in buf

            // Get ID
            IDType id;
            buf >> id;

            ssvu::lo("ID") << id << "\n";

            // Assert ID validity
            assert(id < pcktBindsCount);

            // Call dispatch function
            auto& fn(fncs[id]);
            fn(sender, buf);
        }
    };

    template <typename TConfig>
    class ContextHost
    {
    private:
        using Config = TConfig;
        using MyDispatchTable = DispatchTable<Config>;

        MyDispatchTable dispatchTable;
        nl::ManagedHost managedHost;

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
        void send(const nl::Impl::PayloadTarget& target, T p)
        {
            constexpr nl::UInt32 id(TConfig::template getPcktBindID<T>());
            ssvu::lo("SENT ID") << id << "\n";
            managedHost.send(target, id, FWD(p));
        }

        template <typename TPckt, typename TF>
        void on(TF&& fn)
        {
            dispatchTable.template add<TPckt>(fn);
        }

        template <typename TPckt, typename TF>
        void on_in(TF&& fn)
        {
            dispatchTable.template addDestructured<TPckt>(fn);
        }

        bool busy() const noexcept { return managedHost.isBusy(); }
        auto& host() noexcept { return managedHost; }
    };
}
