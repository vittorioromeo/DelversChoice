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
    class DispatchTable
    {
    private:
        using IDType = typename TConfig::IDType;
        static constexpr auto pcktBindsCount = TConfig::pcktBindsCount;

        using PcktFn =
            std::function<void(nl::Impl::PayloadAddress, nl::PcktBuf&)>;

        std::array<PcktFn, pcktBindsCount> fncs;

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

        void process(const nl::Impl::PayloadAddress& sender, nl::PcktBuf& p)
        {
            // Assumes id is still in buf

            // Get ID
            IDType id;
            p >> id;

            // ssvu::lo("ID") << id << "\n";

            // Assert ID validity
            assert(id < pcktBindsCount);

            // Call dispatch function
            auto& fn(fncs[id]);
            fn(sender, p);
        }
    };
}
