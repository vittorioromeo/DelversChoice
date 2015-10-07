#pragma once

#include "../Common/Common.hpp"
#include "/home/vittorioromeo/OHWorkspace/cppcon2015/Other/Other.hpp"

namespace MPL = ::ecs::MPL;

namespace experiment
{
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
}
