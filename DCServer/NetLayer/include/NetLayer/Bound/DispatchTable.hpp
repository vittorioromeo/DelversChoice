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
        using Config = TConfig;
        using IDType = typename Config::IDType;
        using PcktBuf = nl::PcktBuf;
        using Payload = nl::Impl::Payload;
        using PayloadAddress = nl::Impl::PayloadAddress;

        static constexpr auto pcktBindsCount = Config::pcktBindsCount;

        using PcktFn = std::function<void(const PayloadAddress&, PcktBuf&)>;
        std::array<PcktFn, pcktBindsCount> fncs;

        void process_with_header(const PayloadAddress& sender, PcktBuf& p)
        {
            // Get ID (assumes id is still in buf)
            auto id(nl::make_deserialized<IDType>(p));

            // Assert ID validity
            assert(id < pcktBindsCount);

            // Call dispatch function
            (fncs[id])(sender, p);
        }

    public:
        template <typename T, typename TF>
        void add(TF&& fnToCall)
        {
            constexpr auto id(Config::template getPcktBindID<T>());

            auto& fn(fncs[id]);

            // Assume id has already been taken from
            fn = [id, fnToCall](const PayloadAddress& pt, PcktBuf& buf)
            {
                fnToCall(pt, nl::make_deserialized<T>(buf));
            };
        }

        template <typename T, typename TF>
        void addDestructured(TF&& fnToCall)
        {
            add<T>([fnToCall](auto& pt, auto&& o)
                {
                    auto boundfn([fnToCall, pt](auto&&... xs)
                        {
                            return fnToCall(pt, FWD(xs)...);
                        });

                    ecs::Utils::tupleApply(boundfn, o.fields);
                });
        }

        void process(const PayloadAddress& sender, PcktBuf& p)
        {
            process_with_header(sender, p);
        }
    };
}
