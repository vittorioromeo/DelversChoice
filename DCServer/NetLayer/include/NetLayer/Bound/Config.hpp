#pragma once

#include "../Common/Common.hpp"
#include "/home/vittorioromeo/OHWorkspace/cppcon2015/Other/Other.hpp"
#include "./PcktBind.hpp"

namespace MPL = ::ecs::MPL;

namespace experiment
{
    template <typename TSettings, typename TBindList>
    struct Config
    {
        using IDType = typename TSettings::IDType;
        using Settings = TSettings;
        using PcktBinds = TBindList;

        // static_assert validity of settings
        // static_assert validity of packet binds

        static constexpr std::size_t pcktBindsCount{MPL::size<PcktBinds>()};

        template <typename T>
        static constexpr auto hasPcktBindFor() noexcept
        {
            return MPL::Contains<T, PcktBinds>{};
        }

        template <typename T>
        static constexpr auto getPcktBindID() noexcept
        {
            return static_cast<IDType>(MPL::IndexOf<T, PcktBinds>{});
        }
    };

    template<typename TSettings, typename TBindList>
    constexpr auto make_config(TBindList)
    {
        return Config<TSettings, TBindList>{};
    }
}
